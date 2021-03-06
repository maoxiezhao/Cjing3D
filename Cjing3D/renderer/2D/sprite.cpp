#include "sprite.h"
#include "renderer\textureHelper.h"

namespace Cjing3D
{
	Sprite::Sprite(const std::string& filePath, SpriteType spriteType) :
		mSpriteType(spriteType)
	{
		if (!filePath.empty())
		{
			mFilePath = filePath;
			mTextureResource = GetGlobalContext().gResourceManager->GetOrCreate<TextureResource>(filePath);

			if (mTextureResource->mTexture != nullptr) {
				SetCurrentTextureInfo(*mTextureResource->mTexture);
			}
		}
	}

	Sprite::~Sprite()
	{
	}

	void Sprite::Update(F32 deltaTime)
	{
	}

	void Sprite::FixedUpdate()
	{
	}

	void Sprite::DrawImage(CommandList cmd)
	{
		RenderImage::Render(cmd , *GetTexture(), mImageParams);
	}

	void Sprite::DrawImage(CommandList cmd, const F32x2& pos)
	{
		mImageParams.mPos[0] = pos[0];
		mImageParams.mPos[1] = pos[1];

		DrawImage(cmd);
	}

	bool Sprite::LoadTexture(const std::string& path)
	{
		mTextureResource = GetGlobalContext().gResourceManager->GetOrCreate<TextureResource>(path);
		mFilePath = path;

		if (mTextureResource->mTexture != nullptr) 
		{
			SetCurrentTextureInfo(*mTextureResource->mTexture);
			return true;
		}
		return false;
	}

	void Sprite::SetTexture(TextureResourcePtr res)
	{
		mTextureResource = res;

		if (mTextureResource->mTexture != nullptr) {
			SetCurrentTextureInfo(*mTextureResource->mTexture);
		}
	}

	void Sprite::SetTexture(Texture2D* texture)
	{
		mTextureResource = std::make_shared<TextureResource>();
		mTextureResource->mTexture = texture;
	}

	Texture2D* Sprite::GetTexture()
	{
		return mTextureResource != nullptr ? mTextureResource->mTexture : TextureHelper::GetWhite();
	}

	const Texture2D* Sprite::GetTexture() const
	{
		return mTextureResource != nullptr ? mTextureResource->mTexture : TextureHelper::GetWhite();
	}

	bool Sprite::HasTexture() const
	{
		return mTextureResource != nullptr && mTextureResource->mTexture != nullptr;
	}

	void Sprite::SetSize(F32 w, F32 h)
	{
		mImageParams.mSize[0] = w;
		mImageParams.mSize[1] = h;
	}

	void Sprite::SetSize(const F32x2& size)
	{
		mImageParams.mSize = size;
	}

	void Sprite::SetPos(const F32x2& pos)
	{
		mImageParams.mPos[0] = pos[0];
		mImageParams.mPos[1] = pos[1];
	}

	void Sprite::SetColor(const Color4& color)
	{
		mImageParams.mColor = color.ToFloat4();
	}

	void Sprite::SetColor(const F32x4& color)
	{
		mImageParams.mColor = color;
	}

	Color4 Sprite::GetColor() const
	{
		return Color4(mImageParams.mColor);
	}

	void Sprite::SetNinePatch(const Rect& rect)
	{
		mImageParams.mNinePatchRect = rect;
	}

	Rect Sprite::GetNinePatch() const
	{
		return mImageParams.mNinePatchRect;
	}

	void Sprite::SetCurrentTextureInfo(Texture2D& info)
	{
		const TextureDesc& desc = mTextureResource->mTexture->GetDesc();
		mImageParams.mTexSource[2] = mImageParams.mSize[0] = mTextureSize[0] = (F32)desc.mWidth;
		mImageParams.mTexSource[3] = mImageParams.mSize[1] = mTextureSize[1] = (F32)desc.mHeight;

		mInverseTextureSize[0] = 1.0f / (F32)desc.mWidth;
		mInverseTextureSize[1] = 1.0f / (F32)desc.mHeight;
	}
}