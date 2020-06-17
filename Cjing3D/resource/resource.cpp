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
	mTexture = CJING_MEM_NEW(Texture2D);
}

TextureResource::~TextureResource()
{
	if (mTexture != nullptr)
	{
		mTexture->Clear();
		CJING_MEM_DELETE(mTexture);
	}
}

SoundResource::SoundResource() :
	Resource(Resource_Sound)
{
}

SoundResource::~SoundResource()
{
}

}
