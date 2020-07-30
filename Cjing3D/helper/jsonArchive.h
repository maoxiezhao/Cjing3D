#pragma once


#include "helper\archive.h"
#include "utils\json\json.hpp"
#include "utils\math.h"

#include <stack>
#include <string>
#include <functional>

namespace Cjing3D
{
	namespace JsonArchiveImpl
	{
		template<typename T>
		struct ArchiveType;
	}

	class JsonArchive : public ArchiveBase
	{
	public:
		using JsonSerializerFunc = std::function<void(JsonArchive& archive)>;

		JsonArchive(const std::string& path, ArchiveMode mode);
		~JsonArchive();

		void OnLoad();
		void OnUnload();
		bool Save(const std::string& path) override;

		nlohmann::json* GetCurrentJson();
		const nlohmann::json* GetCurrentJson()const;
		size_t GetCurrentValueCount()const;

		inline void PushMap(const std::string& key, JsonSerializerFunc func)
		{
			auto currentJson = GetCurrentJson();
			if (currentJson == nullptr) {
				return;
			}

			auto it = currentJson->emplace(std::make_pair(key, nlohmann::json())).first;
			mJsonStack.push(&it.value());
			func(*this);
			mJsonStack.pop();
		}

		inline void PushArray(JsonSerializerFunc func)
		{
			auto currentJson = GetCurrentJson();
			if (currentJson == nullptr) {
				return;
			}

			currentJson->emplace_back();
			mJsonStack.push(&currentJson->back());
			func(*this);
			mJsonStack.pop();
		}

		inline void Pop()
		{
			if (!mJsonStack.empty()) {
				mJsonStack.pop();
			}
		}

		template<typename T>
		inline JsonArchive& Write(const std::string& key, const T& data)
		{
			auto currentJson = GetCurrentJson();
			if (currentJson == nullptr) {
				return *this;
			}

			auto it = currentJson->emplace(std::make_pair(key, nlohmann::json())).first;
			mJsonStack.push(&it.value());
			JsonArchiveImpl::ArchiveType<T>::Unserialize(data, *this);
			mJsonStack.pop();

			return *this;
		}

		template<typename T>
		inline JsonArchive& WriteAndPush(const T& data)
		{
			auto currentJson = GetCurrentJson();
			if (currentJson == nullptr) {
				return *this;
			}

			currentJson->emplace_back();
			mJsonStack.push(&currentJson->back());
			JsonArchiveImpl::ArchiveType<T>::Unserialize(data, *this);
			mJsonStack.pop();

			return *this;
		}

		template<typename T>
		inline JsonArchive& Read(const std::string& key, T& data)
		{
			auto currentJson = GetCurrentJson();
			if (currentJson == nullptr) {
				return *this;
			}

			nlohmann::json::iterator it = currentJson->find(key);
			if (it != currentJson->end())
			{
				mJsonStack.push(&it.value());
				JsonArchiveImpl::ArchiveType<T>::Serialize(data, *this);
				mJsonStack.pop();
			}

			return *this;
		}

		template<typename T>
		inline JsonArchive& Read(const size_t index, T& data)
		{
			auto currentJson = GetCurrentJson();
			if (currentJson == nullptr) {
				return *this;
			}

			if (index < 0 || index > currentJson->size()) {
				return *this;
			}

			mJsonStack.push(&currentJson->at(index));
			JsonArchiveImpl::ArchiveType<T>::Serialize(data, *this);
			mJsonStack.pop();

			return *this;
		}

		inline void Read(const size_t index, JsonSerializerFunc func)
		{
			auto currentJson = GetCurrentJson();
			if (currentJson == nullptr) {
				return;
			}

			if (index < 0 || index > currentJson->size()) {
				return;
			}

			mJsonStack.push(&currentJson->at(index));
			func(*this);
			mJsonStack.pop();
		}

		inline void Read(const std::string& key, JsonSerializerFunc func)
		{
			auto currentJson = GetCurrentJson();
			if (currentJson == nullptr) {
				return;
			}

			nlohmann::json::iterator it = currentJson->find(key);
			if (it == currentJson->end()) {
				return;
			}

			mJsonStack.push(&it.value());
			func(*this);
			mJsonStack.pop();
		}

	private:
		std::stack<nlohmann::json*> mJsonStack;
		nlohmann::json mRootJson;

		static const U32 currentArchiveVersion;
	};

	class JsonSerializer
	{
	public:
		virtual void Serialize(JsonArchive& archive) = 0;
		virtual void Unserialize(JsonArchive& archive)const = 0;
	};

	namespace JsonArchiveImpl
	{
		template<typename T, typename ENABLED = void>
		struct ArchiveTypeNormalMapping;

		template<typename T>
		struct ArchiveTypeClassMapping
		{
			static void Serialize(T& obj, JsonArchive& archive)
			{
				obj.Serialize(archive);
			}

			static void Unserialize(const T& obj, JsonArchive& archive)
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

		//////////////////////////////////////////////////////////////////////////////////////////////////
		
		template<typename T>
		struct ArchiveTypeCommonMapping
		{
			static void Serialize(T& obj, JsonArchive& archive)
			{
				nlohmann::json* currentJson = archive.GetCurrentJson();
				if (currentJson == nullptr) {
					return;
				}
				obj = currentJson->get<T>();
			}

			static void Unserialize(const T& obj, JsonArchive& archive)
			{
				nlohmann::json* currentJson = archive.GetCurrentJson();
				if (currentJson == nullptr) {
					return;
				}
				*currentJson = nlohmann::json(obj);
			}
		};
	
		template<> struct ArchiveTypeNormalMapping<int>			  : ArchiveTypeCommonMapping<int> {};
		template<> struct ArchiveTypeNormalMapping<long>		  : ArchiveTypeCommonMapping<long> {};
		template<> struct ArchiveTypeNormalMapping<unsigned int>  : ArchiveTypeCommonMapping<unsigned int> {};
		template<> struct ArchiveTypeNormalMapping<unsigned long> : ArchiveTypeCommonMapping<unsigned long> {};
		template<> struct ArchiveTypeNormalMapping<char>          : ArchiveTypeCommonMapping<char> {};
		template<> struct ArchiveTypeNormalMapping<unsigned char> : ArchiveTypeCommonMapping<unsigned char> {};
		template<> struct ArchiveTypeNormalMapping<I64> : ArchiveTypeCommonMapping<I64> {};
		template<> struct ArchiveTypeNormalMapping<U64> : ArchiveTypeCommonMapping<U64> {};
		template<> struct ArchiveTypeNormalMapping<F32> : ArchiveTypeCommonMapping<F32> {};
		template<> struct ArchiveTypeNormalMapping<F64> : ArchiveTypeCommonMapping<F64> {};


		template<>
		struct ArchiveTypeNormalMapping<bool>
		{
			static void Serialize(bool& obj, JsonArchive& archive)
			{
				nlohmann::json* currentJson = archive.GetCurrentJson();
				if (currentJson == nullptr) {
					return;
				}
				obj = currentJson->get<bool>();
			}

			static void Unserialize(bool obj, JsonArchive& archive)
			{
				nlohmann::json* currentJson = archive.GetCurrentJson();
				if (currentJson == nullptr) {
					return;
				}
				*currentJson = nlohmann::json(obj);
			}
		};

		template<>
		struct ArchiveTypeNormalMapping<std::string>
		{
			static void Serialize(std::string& obj, JsonArchive& archive)
			{
				nlohmann::json* currentJson = archive.GetCurrentJson();
				if (currentJson == nullptr) {
					return;
				}
				obj = currentJson->get<std::string>();
			}

			static void Unserialize(const std::string& obj, JsonArchive& archive)
			{
				nlohmann::json* currentJson = archive.GetCurrentJson();
				if (currentJson == nullptr) {
					return;
				}
				*currentJson = nlohmann::json(obj);
			}
		};

		template<typename T>
		struct ArchiveTypeNormalMapping<std::vector<T>>
		{
			static void Serialize(std::vector<T>& obj, JsonArchive& archive)
			{
				nlohmann::json* currentJson = archive.GetCurrentJson();
				if (currentJson == nullptr) {
					return;
				}
		
				if (!currentJson->is_array()) {
					return;
				}

				for (int i = 0; i < currentJson->size(); i++) {
					archive.Read(i, obj[i]);
				}
			}

			static void Unserialize(const std::vector <T>& obj, JsonArchive& archive)
			{
				nlohmann::json* currentJson = archive.GetCurrentJson();
				if (currentJson == nullptr) {
					return;
				}
			
				for (int i = 0; i < obj.size(); i++) {
					archive.WriteAndPush(obj[i]);
				}
			}
		};

		template<typename T, size_t N>
		struct ArchiveTypeArrayMapping
		{
			static void Serialize(std::array<T, N>& obj, JsonArchive& archive)
			{
				nlohmann::json* currentJson = archive.GetCurrentJson();
				if (currentJson == nullptr) {
					return;
				}

				if (!currentJson->is_array()) {
					return;
				}

				for (int i = 0; i < N; i++) {
					archive.Read(i, obj[i]);
				}
			}

			static void Unserialize(const std::array<T, N>& obj, JsonArchive& archive)
			{
				nlohmann::json* currentJson = archive.GetCurrentJson();
				if (currentJson == nullptr) {
					return;
				}

				for (int i = 0; i < N; i++) {
					archive.WriteAndPush(obj[i]);
				}
			}
		};

		template<> struct ArchiveTypeNormalMapping<F32x2> : ArchiveTypeArrayMapping<F32, 2> {};
		template<> struct ArchiveTypeNormalMapping<F32x3> : ArchiveTypeArrayMapping<F32, 3> {};
		template<> struct ArchiveTypeNormalMapping<F32x4> : ArchiveTypeArrayMapping<F32, 4> {};
		template<> struct ArchiveTypeNormalMapping<U32x2> : ArchiveTypeArrayMapping<U32, 2> {};
		template<> struct ArchiveTypeNormalMapping<U32x3> : ArchiveTypeArrayMapping<U32, 3> {};
		template<> struct ArchiveTypeNormalMapping<U32x4> : ArchiveTypeArrayMapping<U32, 4> {};
		template<> struct ArchiveTypeNormalMapping<I32x2> : ArchiveTypeArrayMapping<I32, 2> {};
		template<> struct ArchiveTypeNormalMapping<I32x3> : ArchiveTypeArrayMapping<I32, 3> {};
		template<> struct ArchiveTypeNormalMapping<I32x4> : ArchiveTypeArrayMapping<I32, 4> {};
	}
}