#pragma once

#include "RHI\rhiResource.h"
#include "utils\color.h"

namespace Cjing3D
{
	namespace TextureHelper
	{
		void Initialize();
		void Uninitialize();

		Texture2D* GetWhite();
		Texture2D* GetColor(const Color4& color);
		
		HRESULT CreateTexture(Texture2D& texture, const uint8_t* data, U32 width, U32 height, FORMAT format);

		void SwapTexture(Texture2D& texture1, Texture2D& texture2);
	}
}