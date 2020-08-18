#pragma once

#include "common\common.h"
#include "core\subSystem.hpp"
#include "helper\variant.h"

#include <functional>

namespace Cjing3D
{
	class Engine;

	class Settings : public SubSystem
	{
	public:
		Settings(GlobalContext& globalContext);
		~Settings();

		void Initialize();
		void Uninitialize()override;
		void Mapping();
		void Reflect();

		void AddSetting(const std::string& key, const Variant& defaultValue, std::function<void(const Variant& newValue)> func = nullptr);
		void SetSetting(const std::string& key, const Variant& value);
		Variant GetSetting(const std::string& key);

	private:
		void Load();
		void Save();

	private:
		struct SetttingItem
		{
			std::string mKey;
			Variant mCurrentValue;
			Variant mDefaultValue;
			std::function<void(const Variant& newValue)> mMappingFunc;

			SetttingItem(const std::string& key, const Variant& defaultValue, std::function<void(const Variant& newValue)> func = nullptr) :
				mKey(key),
				mCurrentValue(defaultValue),
				mDefaultValue(defaultValue),
				mMappingFunc(func) {
			}

		};
		std::map<std::string, std::shared_ptr<SetttingItem>> mSettingItems;
	};

}