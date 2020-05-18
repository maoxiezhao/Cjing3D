#pragma once

#include "system\component\componentInclude.h"

namespace Cjing3D {

	class WeatherComponent : public Component
	{
	public:
		WeatherComponent();
		~WeatherComponent();

		std::string mSkyMapName;
		Texture2DPtr mSkyMap = nullptr;


		void LoadSkyMap(const std::string& name);

		virtual void Serialize(Archive& archive, U32 seed = 0);
		virtual void Unserialize(Archive& archive)const;
	};
}