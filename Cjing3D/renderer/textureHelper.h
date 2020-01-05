#pragma once

#include "renderer\renderableCommon.h"
#include "RHI\rhiResource.h"

namespace Cjing3D
{
	namespace TextureHelper
	{
		void Initialize();
		void Uninitialize();

		Texture2D* GetWhite();
		Texture2D* GetColor(const Color4& color);
		
		HRESULT CreateTexture(Texture2D& texture, const uint8_t* data, U32 width, U32 height, FORMAT format);
	}
}