#pragma once

#include "system\component\componentInclude.h"
#include "helper\fileSystem.h"

namespace Cjing3D {

	class WeatherComponent : public Component
	{
	public:
		WeatherComponent();
		~WeatherComponent();

		std::string mSkyMapName;
		TextureResourcePtr mSkyMap = nullptr;

		void LoadSkyMap(const std::string& name);
		void LoadSkyMap(const I32x2& size, const std::vector<std::filesystem::path>& filePaths);

		virtual void Serialize(Archive& archive, U32 seed = 0);
		virtual void Unserialize(Archive& archive)const;
	};
}