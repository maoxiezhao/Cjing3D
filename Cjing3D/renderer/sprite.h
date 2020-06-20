#pragma once

#include "renderer\paths\renderImage.h"
#include "renderer\renderableCommon.h"
#include "resource\resourceManager.h"

namespace Cjing3D
{
	class Spirite
	{
	public:
		Spirite(const std::string& filePath = "");
		~Spirite();

		virtual void Update(F32 deltaTime);
		virtual void FixedUpdate();

		void Draw();

	private:
		std::string mFilePath;
		TextureResourcePtr mTextureResource;
		RenderImage::ImageParams mImageParams;
	};
}