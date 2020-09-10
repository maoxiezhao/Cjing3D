#include "jsonApi.h"
#include "helper\fileSystem.h"
#include "utils\json\json.hpp"

namespace Cjing3D {
namespace LuaApi {
	namespace
	{
		class BindJsonObject
		{
		public:
			BindJsonObject() {}
			BindJsonObject(const nlohmann::json& json) : mJson(json) {}

			static int ParseJson(lua_State* l)
			{
				return LuaTools::ExceptionBoundary(l, [&] {
					std::string path = LuaTools::Get<std::string>(l, 1);

					if (!FileData::IsFileExists(path))
					{
						Debug::Warning("Can not found json file:" + path);
						return 0;
					}

					size_t length = 0;
					const char* buffer = FileData::ReadFileBytes(path, length);
					if (buffer == nullptr || length < 0)
					{
						Debug::Warning("Filed to open json file:" + path);
						return 0;
					}

					nlohmann::json json;
					try 
					{
						json = nlohmann::json::parse(buffer, buffer + length);
					}
					catch (const std::exception & e)
					{
						Debug::Warning("Filed to open json file:" + path + " ," + e.what());
						return 0;
					}

					LuaTools::Push(l, BindJsonObject(json));
					return 1;
				});
			}

			nlohmann::json& GetJson()
			{
				return mJson;
			}

			bool IsEmpty() const
			{
				return mJson.is_null();
			}

			nlohmann::json::value_t GetValueType()const
			{
				return mJson.type();
			}

			I32 GetChildrenCount()const
			{
				return mJson.size();
			}

			static int GetByIndex(lua_State* l)
			{
				return LuaTools::ExceptionBoundary(l, [&] {
					BindJsonObject& jsonObject = LuaTools::Get<BindJsonObject&>(l, 1);
					I32 index = LuaTools::Get<I32>(l, 2);

					auto& json = jsonObject.GetJson();
					if (index < 0 || index > json.size()) {
						return 0;
					}
					
					nlohmann::json value = json.at(index);
					LuaTools::Push(l, BindJsonObject(value));
					return 1;
				});
			}

			static int GetString(lua_State* l)
			{
				return LuaTools::ExceptionBoundary(l, [&] {
					BindJsonObject& jsonObject = LuaTools::Get<BindJsonObject&>(l, 1);
					if (jsonObject.GetValueType() != nlohmann::json::value_t::string) {
						return 0;
					}

					LuaTools::Push(l, jsonObject.GetJson().get<std::string>());
					return 1;
				});
			}

			static int GetBoolean(lua_State* l)
			{
				return LuaTools::ExceptionBoundary(l, [&] {
					BindJsonObject& jsonObject = LuaTools::Get<BindJsonObject&>(l, 1);
					if (jsonObject.GetValueType() != nlohmann::json::value_t::boolean) {
						return 0;
					}

					LuaTools::Push(l, jsonObject.GetJson().get<bool>());
					return 1;
				});
			}

			static int GetInteger(lua_State* l)
			{
				return LuaTools::ExceptionBoundary(l, [&] {
					BindJsonObject& jsonObject = LuaTools::Get<BindJsonObject&>(l, 1);
					if (jsonObject.GetValueType() != nlohmann::json::value_t::number_integer) {
						return 0;
					}

					LuaTools::Push(l, jsonObject.GetJson().get<I32>());
					return 1;
				});
			}

			static int GetFloat(lua_State* l)
			{
				return LuaTools::ExceptionBoundary(l, [&] {
					BindJsonObject& jsonObject = LuaTools::Get<BindJsonObject&>(l, 1);
					if (jsonObject.GetValueType() != nlohmann::json::value_t::number_float) {
						return 0;
					}

					LuaTools::Push(l, jsonObject.GetJson().get<F32>());
					return 1;
				});
			}

			static int Find(lua_State* l)
			{
				return LuaTools::ExceptionBoundary(l, [&] {
					BindJsonObject& jsonObject = LuaTools::Get<BindJsonObject&>(l, 1);
					std::string name = LuaTools::Get<std::string>(l, 2);
					
					auto& json = jsonObject.GetJson();
					nlohmann::json::const_iterator it = json.find(name);
					if (it == json.end()) {
						return 0;
					}

					;
					LuaTools::Push(l, BindJsonObject(it.value()));
					return 1;
				});
			}
		
		private:
			nlohmann::json mJson;
		};
	}

	void BindJsonModules(lua_State* l)
	{
		LuaBinder(l)
			.BeginClass<BindJsonObject>("Json")
			.AddCFunction("ParseJson", BindJsonObject::ParseJson)
			.AddCFunctionMethod("Find", BindJsonObject::Find)
			.AddCFunctionMethod("GetFloat", BindJsonObject::GetFloat)
			.AddCFunctionMethod("GetInteger", BindJsonObject::GetInteger)
			.AddCFunctionMethod("GetBoolean", BindJsonObject::GetBoolean)
			.AddCFunctionMethod("GetString", BindJsonObject::GetString)
			.AddCFunctionMethod("GetByIndex", BindJsonObject::GetByIndex)
			.AddMethod("GetChildrenCount", &BindJsonObject::GetChildrenCount)
			.AddMethod("GetValueType", &BindJsonObject::GetValueType)
			.AddMethod("IsEmpty", &BindJsonObject::IsEmpty)
			.EndClass();
	}
}
}