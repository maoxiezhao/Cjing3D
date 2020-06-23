#include "textDrawable.h"

namespace Cjing3D
{
	TextDrawable::TextDrawable()
	{
	}

	TextDrawable::~TextDrawable()
	{
	}

	void TextDrawable::Update(F32 deltaTime)
	{
	}

	void TextDrawable::FixedUpdate()
	{
	}

	void TextDrawable::Draw()
	{
	}

	void TextDrawable::Draw(const F32x2& pos)
	{
		mFontParams.mPos[0] = pos[0];
		mFontParams.mPos[1] = pos[1];


	}
}
