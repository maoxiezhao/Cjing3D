#include "resource.h"
#include "resourceManager.h"
#include "renderer\renderableCommon.h"
#include "renderer\RHI\rhiShader.h"

namespace Cjing3D {

Resource::Resource(ResourceType type) :
	mType(type),
	mGUID(GENERATE_ID)
{
}


TextureResource::TextureResource() :
	Resource(Resource_Texture)
{
}

TextureResource::~TextureResource()
{
	if (mTexture != nullptr && mAutoRelease)
	{
		mTexture->Clear();
		CJING_MEM_DELETE(mTexture);
	}
}

Texture2D* TextureResource::CreateNewTexture()
{
	mTexture = CJING_MEM_NEW(Texture2D);
	mAutoRelease = true;
	return mTexture;
}

SoundResource::SoundResource() :
	Resource(Resource_Sound)
{
}

SoundResource::~SoundResource()
{
}

}
