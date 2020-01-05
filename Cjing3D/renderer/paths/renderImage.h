#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\RHI\rhiResource.h"

namespace Cjing3D
{

class Renderer;                                                                                                   
	
namespace RenderImage
{
	struct ImageParams
	{
		enum FLAGS
		{
			EMPTY = 0,
			FULLSCREEN = 1 << 1,
		};
		uint32_t mFlags = EMPTY;

		BlendType mBlendType = BlendType_Alpha;
		F32x3 mPos = { 0.0f, 0.0f, 0.0f };
		F32x2 mSize = { 1.0f, 1.0f };
		F32x2 mScale = { 1.0f, 1.0f };
		F32x4 mColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		F32x2 mCorners[4] = {
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{0.0f, 1.0f},
			{1.0f, 1.0f}
		};

		ImageParams()
		{
		}

		ImageParams(F32 posX, F32 posY, F32 width, F32 height, F32x4 color)
		{
			mPos[0] = posX;
			mPos[1] = posY;
			mScale[0] = width;
			mScale[1] = height;
			mColor = color;
		}

		bool IsFullScreenEnabled()const { return mFlags & FLAGS::FULLSCREEN; }
		void EnableFullScreen() { mFlags |= FLAGS::FULLSCREEN; }
	};

	void Render(RhiTexture2D & texture, ImageParams params, Renderer& renderer);
}
}