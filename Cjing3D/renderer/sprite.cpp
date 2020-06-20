#include "sprite.h"
#include "renderer\textureHelper.h"

namespace Cjing3D
{
	Spirite::Spirite(const std::string& filePath)
	{
		if (!filePath.empty())
		{
			mFilePath = filePath;
			mTextureResource = GlobalGetSubSystem<ResourceManager>().GetOrCreate<TextureResource>(filePath);
		}
	}

	Spirite::~Spirite()
	{
	}

	void Spirite::Update(F32 deltaTime)
	{
	}

	void Spirite::FixedUpdate()
	{
	}

	void Spirite::Draw()
	{
		//Texture2D* texture = mTextureResource != nullptr ? mTextureResource->mTexture : TextureHelper::GetWhite();
		//RenderImage::Render(*texture, mImageParams,)
	}
}