#include "settings.h"

#include <fstream>
#include <iostream>

namespace Cjing3D
{
    namespace 
    {
		const std::string settingFileName = "Cjing3D.ini";
    }

    Settings::Settings(GlobalContext& globalContext) :
        SubSystem(globalContext)
    {
    }

    Settings::~Settings()
    {
    }

    void Settings::Initialize()
    {
    }

    void Settings::Uninitialize()
    {
    }

    void Settings::Mapping()
    {

    }
    void Settings::Reflect()
    {
    }

    void Settings::Load()
    {
    }

    void Settings::Save()
    {
    }
}