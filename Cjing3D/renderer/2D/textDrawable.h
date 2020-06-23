#pragma once

#include "renderer\2D\font.h"

namespace Cjing3D
{
	class TextDrawable
	{
	public:
		TextDrawable();
		~TextDrawable();

		virtual void Update(F32 deltaTime);
		virtual void FixedUpdate();

		void Draw();
		void Draw(const F32x2& pos);

		bool IsVisible()const { return mIsVisible; }

	private:
		Font::FontParams mFontParams;
		bool mIsVisible = true;
	};
}