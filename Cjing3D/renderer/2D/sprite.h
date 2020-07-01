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
		bool LoadTexture(const std::string& path);
		void SetTexture(TextureResourcePtr res);
		void SetTexture(Texture2DPtr texture);

		Texture2D* GetTexture();
		const Texture2D* GetTexture()const;
		TextureResourcePtr GetTextureResource() { return mTextureResource; }
		RenderImage::ImageParams& GetImageParams() { return mImageParams; }
		F32x2 GetTextureSize()const { return mTextureSize; }
		F32x2 GetInverseTextureSize()const { return mInverseTextureSize; }
		bool HasTexture()const;

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
		RenderImage::ImageParams mImageParams;
		bool mIsVisible = true;
		F32x2 mTextureSize = { 1.0f, 1.0f };
		F32x2 mInverseTextureSize = { 1.0f, 1.0f };

		// TODO sprite 应该还是仅保持texturePtr指针
		TextureResourcePtr mTextureResource = nullptr;
		//Texture2DPtr mTexture = nullptr;
	};
}