#pragma once

#include "renderer\renderableCommon.h"
#include "resource\resource.h"
#include "renderer\RHI\rhiTexture.h"

namespace Cjing3D
{
	class Material : public Resource
	{
	public:
		Material();
		~Material();

		RhiTexture2DPtr GetBaseColorMap() { return mBaseColorMap; }
		RhiTexture2DPtr GetSurfaceMap() { return mSurfaceMap; }
		RhiTexture2DPtr GetNormalMap() { return mNormalMap; }
	private:

		RhiTexture2DPtr mBaseColorMap;
		RhiTexture2DPtr mSurfaceMap;
		RhiTexture2DPtr mNormalMap;
	};

	using MaterialPtr = std::shared_ptr<Material>;

}