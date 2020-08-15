#pragma once

#include "common\common.h"
#include "core\subSystem.hpp"

namespace Cjing3D
{
	class Settings : public SubSystem
	{
	public:
		std::string mTitleName = "";
		I32x2 mResolution = I32x2(0, 0);
		bool mIsFullScreen = false;

	public:
		Settings(GlobalContext& globalContext);
		~Settings();

		void Initialize();
		void Uninitialize()override;
		void Mapping();
		void Reflect();

	private:
		void Load();
		void Save();


	};
}