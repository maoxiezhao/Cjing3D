#pragma once

#include "common\common.h"
#include "helper\stringID.h"
#include "renderer\RHI\rhiResource.h"
#include "audio\audioDevice.h"

namespace Cjing3D
{
	class ResourceManager;

	enum ResourceType
	{
		Resource_Unknown,
		Resource_Texture,
		Resource_Shader,
		Resource_Model,
		Resource_Sound,
	};

	/**
	*	\brief 资源类基类
	*/
	class Resource : public std::enable_shared_from_this<Resource>
	{
	public:
		Resource(ResourceType type);
		Resource(const Resource& rhs) = delete;
		Resource(Resource&& rhs) = default;
		virtual ~Resource() {};

		Resource& operator= (const Resource& other) = delete;
		Resource& operator= (Resource&& other) = default;

		ResourceType GetResourceType()const { return mType; }
		U32 GetGUID()const { return mGUID; }
	
	private:
		U32 mGUID;
		ResourceType mType;
	};

	using ResourcePtr = std::shared_ptr<Resource>;

	class TextureResource : public Resource
	{
	public:
		TextureResource();
		~TextureResource();

		Texture2D* mTexture = nullptr;
	};
	using TextureResourcePtr = std::shared_ptr<TextureResource>;

	class SoundResource : public Resource
	{
	public:
		SoundResource();
		~SoundResource();

		Audio::SoundResource mSound;
	};
	using SoundResourcePtr = std::shared_ptr<SoundResource>;
}