#pragma once


#include "helper\archive.h"
#include "utils\json\json.hpp"

#include <stack>
#include <string>

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
		JsonArchive(const std::string& path, ArchiveMode mode);
		~JsonArchive();

		void OnLoad();
		void OnUnload();
		bool Save(const std::string& path) override;

		nlohmann::json* GetCurrentJson();
		const nlohmann::json* GetCurrentJson()const;
		size_t GetCurrentValueCount()const;

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

			nlohmann::json::const_iterator it = currentJson->find(key);
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
				return 0;
			}

			mJsonStack.push(&currentJson->at(index));
			JsonArchiveImpl::ArchiveType<T>::Serialize(data, *this);
			mJsonStack.pop();

			return *this;
		}

	private:
		std::stack<nlohmann::json*> mJsonStack;
		nlohmann::json mRootJson;

		static const U32 currentArchiveVersion;
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
		template<typename T, typename ENABLED>
		struct ArchiveTypeNormalMapping
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

				size_t count = currentJson->size();
				for (int i = 0; i < count; i++) {
					archive.Read(i, obj[i]);
				}
			}

			static void Unserialize(const std::vector <T>& obj, JsonArchive& archive)
			{
				nlohmann::json* currentJson = archive.GetCurrentJson();
				if (currentJson == nullptr) {
					return;
				}
			
				size_t index = 0;
				for (const auto& item : obj) {
					archive.Write(index++, item);
				}
			}
		};
	}
}