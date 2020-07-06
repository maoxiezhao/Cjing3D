#include "ninePatchImage.h"
#include "widgetInclude.h"

namespace Cjing3D
{
namespace Gui
{
	NinePatchImage::NinePatchImage(GUIStage& stage, const StringID& name, const std::string& path) :
		Widget(stage, name),
		mImage(path)
	{
		mImage.SetSpriteType(Sprite::SpriteType_NinePatch);
	}

	void NinePatchImage::SetTexture(const std::string& path)
	{
		if (mImage.LoadTexture(path)) {
			OnTextureChanged();
		}
	}

	void NinePatchImage::SetTexture(const Sprite& sprite)
	{
		if (sprite.GetTexture() != mImage.GetTexture())
		{
			mImage = sprite;
			OnTextureChanged();
		}
	}

	void NinePatchImage::SetTexture(TextureResourcePtr texture)
	{
		if (mImage.GetTextureResource() != texture)
		{
			mImage.SetTexture(texture);
			OnTextureChanged();
		}
	}

	Sprite NinePatchImage::GetSprite()
	{
		return mImage;
	}

	Texture2D* NinePatchImage::GetTexture()
	{
		return mImage.GetTexture();
	}

	void NinePatchImage::SetPatchRect(const Rect& rect)
	{
		mPatchRect = rect;
	}

	void NinePatchImage::SetPatchRect(Rect::RectMargin margin, F32 value)
	{
		mPatchRect.SetMargin(margin, value);
	}

	F32x2 NinePatchImage::CalculateBestSize() const
	{
		F32x2 size = Widget::CalculateBestSize();
		F32x2 minSize = {
			mPatchRect.mLeft + mPatchRect.mRight,
			mPatchRect.mTop  + mPatchRect.mBottom
		};
		return F32x2Max(minSize, size);
	}

	void NinePatchImage::RenderImpl(const Rect& destRect)
	{
		GUIRenderer& renderer = GetGUIRenderer();
		const GUIScheme& scheme = renderer.GetGUIScheme();

		mImage.SetPos(destRect.GetPos());
		if (!mImage.HasTexture())
		{
			// 颜色块texture直接按大小渲染
			mImage.SetSize(destRect.GetSize());
			renderer.RenderSprite(mImage);
		}
		else
		{
			F32x2 targetSize = destRect.GetSize();
			mImage.SetSize(destRect.GetSize());
			mImage.SetNinePatch(mPatchRect);
			renderer.RenderSprite(mImage);
		}
	}
}
}