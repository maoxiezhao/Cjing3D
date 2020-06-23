#pragma once

#include "renderer\paths\renderImage.h"
#include "renderer\renderableCommon.h"
#include "resource\resourceManager.h"

namespace Cjing3D
{
	namespace Font
	{
		struct FontParams
		{
			enum TextAlignH
			{
				TextAlignH_LEFT,
				TextAlignH_Center,
				TextAlignH_Right,
			};
			TextAlignH mTextAlignH = TextAlignH_LEFT;
			enum TextAlignV
			{
				TextAlignV_Up,
				TextAlignV_Center,
				TextAlignV_Bottom,
			};
			TextAlignV mTextAlignV = TextAlignV_Up;

			F32x2 mPos = 0.0f;
			Color4 mColor = Color4::White();
			U32 mFontStyleIndex = 0;
			U32 mFontSize = 16;
			F32 mLineSpacing = 1.0f;
			F32 mColSpacing = 1.0f;
			F32 mScale = 1.0f;
		};

		void Initialize();
		void Uninitialize();
		void LoadFontTTF(const std::string& ttfFile);

		// TODO
		void Draw(const std::string& text, const FontParams& params);
		void Draw(const std::wstring& text, const FontParams& params);

		const U32 MaxFontRenderLength = 1000;
		const U32 MaxLoadedFontCount = 15;
	}
}