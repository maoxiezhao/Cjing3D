#pragma once

#include "renderer\RHI\rhiTexture.h"
#include "resource\resource.h"

namespace Cjing3D
{
	/**
	*	\brief high-level texture
	*/
	class Texture2D : public Resource
	{
	public:
		Texture2D();
		virtual ~Texture2D();


		RhiTexture2DPtr GetRhiTexture() {
			return mRhiTexture;
		}

	private:
		RhiTexture2DPtr mRhiTexture;
	};
}