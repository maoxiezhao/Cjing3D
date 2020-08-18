#include "settings.h"
#include "helper\fileSystem.h"
#include "helper\jsonArchive.h"
#include "engine.h"
#include "platform\gameWindow.h"

#include <fstream>
#include <iostream>

namespace Cjing3D
{
    const std::string settingFileName = "setting.ini";

    Settings::Settings(GlobalContext& globalContext) :
        SubSystem(globalContext)
    {
        AddSetting("isFullScreen", false);
        AddSetting("resolution", I32x2(DEFAULT_GAME_WINDOW_WIDTH, DEFAULT_GAME_WINDOW_HEIGHT));
    }

    Settings::~Settings()
    {
    }

    void Settings::Initialize()
    {
        Engine* engine = GlobalGetEngine();
        if (engine == nullptr) 
        {
            Debug::Error("Settting::init error: Invalid engine;");
            return;
        }

        GameWindow* gameWindow = engine->GetWindow();
		if (gameWindow == nullptr)
		{
			Debug::Error("Settting::init error: Engine must set game window;");
			return;
		}

        if (FileData::IsFileExists(settingFileName)) {
            Load();
        }
        else {
            Save();
        }

        // 初次加载时，其他subSystem还未加载，所以这里直接修改初始参数，而不做Mapping
        auto resolution = GetSetting("resolution");
        if (!resolution.IsEmpty()) 
        {
            auto size = resolution.GetValue<I32x2>();
            gameWindow->SetWindowSize(size);
            Logger::InfoEx("[Setting] Resolution: %dx%d", size[0], size[1]);
        }

		auto isFullScreen = GetSetting("isFullScreen");
		if (!isFullScreen.IsEmpty()) {
			gameWindow->SetIsWindowFullScreen(isFullScreen.GetValue<bool>());
		}
    }

    void Settings::Uninitialize()
    {
        mSettingItems.clear();
    }

    void Settings::Mapping()
    {

    }
    void Settings::Reflect()
    {
    }

    void Settings::AddSetting(const std::string& key, const Variant& defaultValue, std::function<void(const Variant& newValue)> func)
    {
        mSettingItems[key] = CJING_MAKE_SHARED<SetttingItem>(key, defaultValue, func);
    }

    void Settings::SetSetting(const std::string& key, const Variant& value)
    {
        auto it = mSettingItems.find(key);
        if (it == mSettingItems.end()) 
        {
            Debug::Warning("Settings::SetSetting: Invalid setting key.");
            return;
        }

        it->second->mCurrentValue = value;
    }

    Variant Settings::GetSetting(const std::string& key)
    {
		auto it = mSettingItems.find(key);
		if (it == mSettingItems.end()) 
        {
			Debug::Warning("Settings::SetSetting: Invalid setting key.");
            return Variant();
		}

        return it->second->mCurrentValue;
    }

    void Settings::Load()
    {
		JsonArchive archive(settingFileName, ArchiveMode::ArchiveMode_Read);
		size_t count = archive.GetCurrentValueCount();
		for (size_t index = 0; index < count; index++)
		{
			archive.Read(index, [this](JsonArchive& archive)
			{
                std::string key;
				archive.Read("key", key);
                Variant value;
				archive.Read("val", value);

                SetSetting(key, value);
			});
		}
    }

    void Settings::Save()
    {
       JsonArchive archive(settingFileName, ArchiveMode::ArchiveMode_Write);
	   for (auto kvp : mSettingItems)
	   {
		   archive.PushArray([&kvp](JsonArchive& archive)
			{
				archive.Write("key", kvp.first);
				archive.Write("val", kvp.second->mCurrentValue);
			});
	   }
    }
}