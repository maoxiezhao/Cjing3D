#pragma once

#include "world\component\componentInclude.h"

namespace Cjing3D {

	class MaterialComponent : public Component
	{
	public:
		MaterialComponent() : Component(ComponentType_Material) {}

		F32x4 mBaseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		F32 mRoughness = 0.2f;
		F32 mMetalness = 0.0f;

		std::string mBaseColorMapName;
		std::string mSurfaceMapName;
		std::string mNormalMapName;

		RhiTexture2DPtr mBaseColorMap = nullptr;
		RhiTexture2DPtr mSurfaceMap = nullptr;
		RhiTexture2DPtr mNormalMap = nullptr;

		inline bool IsCastingShadow()const { return mIsCastingShadow; }

		bool mIsCastingShadow = false;
	};

}

