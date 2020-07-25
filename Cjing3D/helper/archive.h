#pragma once

#include "common\common.h"

#include <vector>

namespace Cjing3D {

	class Archive;

	template<typename T, typename ENABLED = void>
	struct ArchiveTypeNormalMapping;

	template<typename T>
	struct ArchiveTypeClassMapping
	{
		static void Serialize(T& obj, Archive& archive)
		{
			obj.Serialize(archive);
		}

		static void Unserialize(const T& obj, Archive& archive)
		{
			obj.Unserialize(archive);
		}
	};

	struct ArchiveTypeExists
	{
		// 通过typename = decltype(T())判断是否存在该类型
		template<typename T, typename = decltype(T())>
		static std::true_type test(int);

		template<typename>
		static std::false_type test(...);
	};

	template<typename T>
	struct ArchiveTypeMappingExists
	{
		using type = decltype(ArchiveTypeExists::test<ArchiveTypeNormalMapping<T>>(0));
		static constexpr bool value = type::value;
	};

	template<typename T>
	struct ArchiveType :
		std::conditional<
		std::is_class<typename std::decay<T>::type>::value &&
			!(ArchiveTypeMappingExists<typename std::decay<T>::type>::value),
		ArchiveTypeClassMapping<typename std::decay<T>::type>,
		ArchiveTypeNormalMapping<typename std::decay<T>::type> >::type
	{};

	///////////////////////////////////////////////////////////////////////////////////////

	enum class ArchiveMode
	{
		ArchiveMode_Read,
		ArchiveMode_Write
	};

	class Archive
	{
	public:
		Archive(const std::string& path, ArchiveMode mode);
		~Archive();

		bool IsOpen()const;
		void Close();
		void Save(const std::string& path);
		std::string GetDirectory()const;

		template<typename T>
		inline Archive& operator << (const T& data)
		{
			ArchiveType<T>::Unserialize(data, *this);
			return *this;
		}

		template<typename T>
		inline Archive& operator >> (T& data)
		{
			ArchiveType<T>::Serialize(data, *this);
			return *this;
		}

		template<typename T>
		inline void Write(const T& data, U32 count = 1)
		{
			U32 size = sizeof(data) * count;
			WriteImpl(&data, size);
		}

		template<typename T>
		inline void Read(T& data, U32 count = 1)
		{
			U32 size = sizeof(data) * count;
			ReadImpl(&data, size);
		}

	private:
		void WriteImpl(const void* data, U32 size);
		void ReadImpl(void* data, U32 size);

	private:
		ArchiveMode mMode = ArchiveMode::ArchiveMode_Read;
		std::string mFilePath;

		char* mDataBuffer = nullptr;
		U32 mDataSize = 0;
		U32 mReadPos = 0;
		U32 mCurrentArchiveVersion;

		static U32 currentArchiveVersion;
	};

	template<typename T, typename EXTRA_T = T>
	struct ArchiveTypeExtraTypeMapping
	{
		static void Serialize(T& obj, Archive& archive)
		{
			EXTRA_T temp;
			archive.Read<EXTRA_T>(temp);
			obj = static_cast<T>(temp);
		}

		static void Unserialize(const T& obj, Archive& archive)
		{
			archive.Write<EXTRA_T>(static_cast<EXTRA_T>(obj));
		}
	};

	// base type
	template<> struct ArchiveTypeNormalMapping<int> : ArchiveTypeExtraTypeMapping<int, I64> {};
	template<> struct ArchiveTypeNormalMapping<long> : ArchiveTypeExtraTypeMapping<long, I64> {};
	template<> struct ArchiveTypeNormalMapping<unsigned int> : ArchiveTypeExtraTypeMapping<unsigned int, U64> {};
	template<> struct ArchiveTypeNormalMapping<unsigned long> : ArchiveTypeExtraTypeMapping<unsigned long, U64> {};
	template<> struct ArchiveTypeNormalMapping<char> : ArchiveTypeExtraTypeMapping<char, I8> {};
	template<> struct ArchiveTypeNormalMapping<unsigned char> : ArchiveTypeExtraTypeMapping<unsigned char, U8> {};

	template<> struct ArchiveTypeNormalMapping<I64> : ArchiveTypeExtraTypeMapping<I64> {};
	template<> struct ArchiveTypeNormalMapping<U64> : ArchiveTypeExtraTypeMapping<U64> {};
	template<> struct ArchiveTypeNormalMapping<F32> : ArchiveTypeExtraTypeMapping<F32> {};
	template<> struct ArchiveTypeNormalMapping<F64> : ArchiveTypeExtraTypeMapping<F64> {};

	// vector type
	template<> struct ArchiveTypeNormalMapping<XMFLOAT3> : ArchiveTypeExtraTypeMapping<XMFLOAT3> {};
	template<> struct ArchiveTypeNormalMapping<XMFLOAT4> : ArchiveTypeExtraTypeMapping<XMFLOAT4> {};
	template<> struct ArchiveTypeNormalMapping<XMFLOAT4X4> : ArchiveTypeExtraTypeMapping<XMFLOAT4X4> {};
	
	template<> struct ArchiveTypeNormalMapping<F32x2> : ArchiveTypeExtraTypeMapping<F32x2> {};
	template<> struct ArchiveTypeNormalMapping<F32x3> : ArchiveTypeExtraTypeMapping<F32x3> {};
	template<> struct ArchiveTypeNormalMapping<F32x4> : ArchiveTypeExtraTypeMapping<F32x4> {};

	template<> struct ArchiveTypeNormalMapping<U32x2> : ArchiveTypeExtraTypeMapping<U32x2> {};
	template<> struct ArchiveTypeNormalMapping<U32x3> : ArchiveTypeExtraTypeMapping<U32x3> {};
	template<> struct ArchiveTypeNormalMapping<U32x4> : ArchiveTypeExtraTypeMapping<U32x4> {};

	// enum type
	template<typename T>
	struct ArchiveTypeNormalMapping<T, typename std::enable_if<std::is_enum<T>::value, void>::type> : 
		ArchiveTypeExtraTypeMapping<T, U64> {};

	template<>
	struct ArchiveTypeNormalMapping<bool>
	{
		static void Serialize(bool& obj, Archive& archive)
		{
			U32 temp;
			archive.Read<U32>(temp);
			obj = (temp == 1);
		}

		static void Unserialize(const bool& obj, Archive& archive)
		{
			archive.Write<U32>(obj ? 1 : 0);
		}
	};

	template<>
	struct ArchiveTypeNormalMapping<std::string>
	{
		static void Serialize(std::string& obj, Archive& archive)
		{
			U32 length = 0;
			archive >> length;

			if (length > 0)
			{
				char* strBuffer = new char[length];
				memset(strBuffer, '\0', sizeof(char) * length);

				archive.Read(*strBuffer, length);
				obj = std::string(strBuffer);
				SAFE_DELETE_ARRAY(strBuffer);
			}
		}

		static void Unserialize(const std::string& obj, Archive& archive)
		{
			U32 length = obj.length() + 1; //  +1 for '\0'
			archive << length;
			archive.Write(*obj.c_str(), length);
		}
	};

	template<typename T>
	struct ArchiveTypeNormalMapping<std::vector<T>>
	{
		static void Serialize(std::vector<T>& obj, Archive& archive)
		{
			U32 length = 0;
			archive >> length;

			obj.resize(length);
			for (int i = 0; i < length; i++) {
				archive >> obj[i];
			}
		}

		static void Unserialize(const std::vector<T>& obj, Archive& archive)
		{
			archive << obj.size();
			for (auto& item : obj) {
				archive << item;
			}
		}
	};
}