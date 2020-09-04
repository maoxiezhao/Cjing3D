#pragma once

#include "renderer\paths\renderImage.h"
#include "renderer\renderableCommon.h"
#include "resource\resourceManager.h"

namespace Cjing3D
{
	class Sprite
	{
	public:
		enum SpriteType
		{
			SpriteType_Normal,
			SpriteType_NinePatch,
		};

		Sprite(const std::string& filePath = "", SpriteType spriteType = SpriteType_Normal);
		~Sprite();

		virtual void Update(F32 deltaTime);
		virtual void FixedUpdate();

		void DrawImage(CommandList cmd);
		void DrawImage(CommandList cmd, const F32x2& pos);
		bool LoadTexture(const std::string& path);
		void SetTexture(TextureResourcePtr res);
		void SetTexture(Texture2D* texture);

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
		void SetNinePatch(const Rect& rect);
		Rect GetNinePatch()const;

		void SetSpriteType(SpriteType type) { mSpriteType = type; }
		SpriteType GetSpriteType()const { return mSpriteType; }

	private:
		void SetCurrentTextureInfo(Texture2D& info);

	private:
		SpriteType mSpriteType = SpriteType_Normal;
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