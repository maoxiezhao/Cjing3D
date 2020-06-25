#pragma once

#include "renderer\paths\renderImage.h"
#include "renderer\renderableCommon.h"
#include "resource\resourceManager.h"

namespace Cjing3D
{
	class Sprite
	{
	public:
		Sprite(const std::string& filePath = "");
		~Sprite();

		virtual void Update(F32 deltaTime);
		virtual void FixedUpdate();

		void DrawImage();
		void DrawImage(const F32x2& pos);

		Texture2D* GetTexture();
		TextureResourcePtr GetTextureResource() { return mTextureResource; }
		RenderImage::ImageParams& GetImageParams() { return mImageParams; }
		F32x2 GetTextureSize()const { return mTextureSize; }
		F32x2 GetInverseTextureSize()const { return mInverseTextureSize; }

		bool IsVisible()const { return mIsVisible; }
		void SetVisible(bool isVisible) { mIsVisible = isVisible; }
		void SetSize(F32 w, F32 h);
		void SetSize(const F32x2& size);
		void SetPos(const F32x2& pos);
		void SetColor(const Color4& color);
		void SetColor(const F32x4& color);
		Color4 GetColor()const;

	private:
		void SetCurrentTextureInfo(Texture2D& info);

	private:
		std::string mFilePath;
		TextureResourcePtr mTextureResource;
		RenderImage::ImageParams mImageParams;

		bool mIsVisible = true;
		F32x2 mTextureSize = {1.0f, 1.0f};
		F32x2 mInverseTextureSize = {1.0f, 1.0f};
	};
}