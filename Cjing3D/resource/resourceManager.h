#pragma once

#include "core\subSystem.hpp"
#include "helper\fileSystem.h"
#include "resource\resource.h"
#include "resource\resourcePool.hpp"
#include "resource\modelImporter\modelImporter.h"

namespace Cjing3D
{
/**
*	\brief 资源管理器类
*	
*	SUPPORT:
*		Resource_Texture,
*		Resource_Sound
*/
class ResourceManager : public SubSystem
{
public:
	template <typename ResourceT>
	using PoolType = ResourcePool < StringID, ResourceT>;
	template <typename ResourceT>
	using KeyType = typename PoolType<ResourceT>::KeyType;
	template <typename ResourceT>
	using PoolResourceType = typename PoolType<ResourceT>::PoolResourceType;


	ResourceManager(GlobalContext& gameContext) :
		SubSystem(gameContext) {}
	~ResourceManager() = default;

	void Initialize();
	void Uninitialize()override;
	
	void AddStandardResourceDirectory(ResourceType type, const std::string& path);
	const std::string& GetStandardResourceDirectory(ResourceType type)const;

	//----------------------------------------------------------------
	//	Common Method
	//----------------------------------------------------------------
	template <typename ResourceT>
	std::shared_ptr<typename PoolResourceType<ResourceT> > Get(const typename KeyType<ResourceT>& guid)
	{
		auto& shaderPool = GetPool< ResourceT >();
		return shaderPool.Get(guid);
	}

	template <typename ResourceT>
	bool Contains(const typename KeyType<ResourceT>& guid)const
	{
		auto& shaderPool = GetPool< ResourceT >();
		return shaderPool.Contains(guid);
	}

	ResourcePtr GetOrCreateByType(const StringID& name, ResourceType type);

	template<typename ResourceT>
	void ClearResource(const StringID& name)
	{
		PoolType<ResourceT>& pool = GetPool<ResourceT>();
		pool.Remove(name);
	}

	template<typename ResourceT>
	void ClearResources()
	{
		PoolType<ResourceT>& pool = GetPool<ResourceT>();
		pool.RemoveAll();
	}

	//----------------------------------------------------------------
	//	Creating Method
	//----------------------------------------------------------------
	template<typename ResourceT>
	std::shared_ptr< ResourceT> GetOrCreateEmptyResource(const StringID& name);

	template<typename ResourceT>
	std::enable_if_t<std::is_same<ResourceT, TextureResource>::value, std::shared_ptr<TextureResource>>
		GetOrCreate(const StringID& filePath, FORMAT textureFormat = FORMAT_R8G8B8A8_UNORM, U32 channelCount = 4, U32 bindFlag = BIND_SHADER_RESOURCE, bool generateMipmap = false);

	template<typename ResourceT>
	std::enable_if_t<std::is_same<ResourceT, SoundResource>::value, std::shared_ptr<SoundResource>>
		GetOrCreate(const StringID& filePath);

public:
	template <typename ResourceT>
	PoolType<ResourceT>& GetPool()
	{
		return const_cast<PoolType<ResourceT>&>(
			static_cast<const ResourceManager*>(this)->GetPool<ResourceT>());
	}

	template <typename ResourceT>
	const PoolType<ResourceT>& GetPool()const;

	void LoadTextureFromFilePath(const std::filesystem::path& filePath, Texture2D& texture, FORMAT textureFormat = FORMAT_R8G8B8A8_UNORM, U32 channelCount = 4, U32 bindFlag = BIND_SHADER_RESOURCE, bool generateMipmap = false);
	void LoadCubeTextureFromFilePath(const std::vector<std::filesystem::path>& filePaths, Texture2D& texture, const I32x2& size, FORMAT textureFormat = FORMAT_R8G8B8A8_UNORM, U32 bindFlag = BIND_SHADER_RESOURCE, bool generateMipmap = false);
	
private:
	void LoadSoundFromFilePath(const std::filesystem::path& filePath, SoundResource& soundResource);

private:
	std::map<ResourceType, std::string> mResourceDirectories;

	PoolType<TextureResource> mTexture2DPool;
	PoolType<SoundResource> mSoundPool;
};

#include "resource\resourceManager.inl"

}