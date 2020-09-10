#include "image.h"
#include "widgetInclude.h"

namespace Cjing3D
{
namespace Gui
{
	Image::Image(GUIStage& stage, const StringID& name, const std::string& path) :
		Widget(stage, name),
		mSprite(path)
	{
	}

	void Image::SetTexture(const std::string& path)
	{
		if (mSprite.LoadTexture(path)) {
			OnTextureChanged();
		}
	}

	void Image::SetTexture(const Sprite& sprite)
	{
		if (sprite.GetTexture() != mSprite.GetTexture())
		{
			mSprite = sprite;
			OnTextureChanged();
		}
	}

	void Image::SetTexture(TextureResourcePtr texture)
	{
		if (mSprite.GetTextureResource() != texture)
		{
			mSprite.SetTexture(texture);
			OnTextureChanged();
		}
	}

	Sprite Image::GetSprite()
	{
		return mSprite;
	}

	Texture2D* Image::GetTexture()
	{
		return mSprite.GetTexture();
	}

	F32x2 Image::CalculateBestSize() const
	{
		F32x2 bestSize = Widget::CalculateBestSize();
		
		// !mIsExpand,保证最小大小为texture原始大小
		if (!mIsExpand)
		{
			F32x2 texSize = mSprite.GetTextureSize();
			bestSize = F32x2Max(bestSize, texSize);
		}

		return bestSize;
	}

	void Image::RenderImpl(const Rect& destRect)
	{
		GUIRenderer& renderer = GetGUIRenderer();
		const GUIScheme& scheme = renderer.GetGUIScheme();

		mSprite.SetPos(destRect.GetPos());
		mSprite.SetColor(mImageColor);

		if (!mSprite.HasTexture())
		{
			// 颜色块texture直接按大小渲染
			mSprite.SetSize(destRect.GetSize());
			renderer.RenderSprite(mSprite);
		}
		else
		{
			F32x2 targetSize = destRect.GetSize();
			switch (mStretchMode)
			{
			case STRETCH_SCALE_ON_EXPAND:
				targetSize = mIsExpand ? targetSize : mSprite.GetTextureSize();
			case STRETCH_KEEP:
				targetSize = mSprite.GetTextureSize();
				break;
			case STRETCH_KEEP_ASPECT:
				{
					F32x2 texSize = mSprite.GetTextureSize();
					F32 width = texSize[0] * targetSize[1] / texSize[1];
					F32 height = targetSize[1];
					if (width > targetSize[0])
					{
						width = targetSize[0];
						height = texSize[1] * targetSize[0] / texSize[0];
					}
					targetSize = F32x2(width, height);
				} 
				break;
			case STRETCH_SCALE:
			case STRETCH_TILE:
				break;
			default:
				break;
			}
			mSprite.SetSize(destRect.GetSize());
			renderer.RenderSprite(mSprite);
		}
	}
}
}