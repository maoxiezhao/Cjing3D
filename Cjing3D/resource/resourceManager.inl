#include "resourceManager.h"

/**
*	\brief 获取指定资源
*	\return 如果不存在则返回nullptr
*/
template<typename ResourceT>
inline std::shared_ptr<typename ResourceManager::ResourceType<ResourceT> >
ResourceManager::Get(const KeyType<ResourceT>& guid)
{
	auto& shaderPool = GetPool< ResourceT >();
	return shaderPool.Get(guid);
}

/**
*	\brief 判断是否包含指定资源
*/
template<typename ResourceT>
inline bool ResourceManager::Contains(const typename KeyType<ResourceT>& guid) const
{
	auto& shaderPool = GetPool< ResourceT >();
	return shaderPool.Contains(guid);
}

/**
*	\brief 返回指定资源池
*/
template<typename ResourceT>
inline ResourceManager::PoolType<ResourceT>& ResourceManager::GetPool()
{
	return const_cast<PoolType<ResourceT>&>(
		static_cast<const ResourceManager*>(this)->GetPool<ResourceT>());
}

template <>
inline const ResourceManager::PoolType<RhiTexture2D>&
ResourceManager::GetPool<RhiTexture2D>()const
{
	return mTexture2DPool;
}

template<typename ResourceT>
inline std::enable_if_t<std::is_same<ResourceT, RhiTexture2D>::value, std::shared_ptr<RhiTexture2D>>
ResourceManager::GetOrCreate(const StringID & filePath)
{
	PoolType<RhiTexture2D>& texturePool = GetPool< RhiTexture2D >();

	bool isExists = texturePool.Contains(filePath);
	auto texture = texturePool.GetOrCreate(filePath);
	if (isExists == false) {
		LoadTextureFromFilePath(filePath.GetString(), *texture);
	}
	return texture;
}


template<typename ResourceT>
inline std::enable_if_t<std::is_same<ResourceT, RhiTexture2D>::value, std::shared_ptr<RhiTexture2D>> 
ResourceManager::GetOrCreate(const StringID& filePath, FORMAT textureFormat, U32 channelCount, U32 bindFlag, bool generateMipmap)
{
	PoolType<RhiTexture2D>& texturePool = GetPool< RhiTexture2D >();

	bool isExists = texturePool.Contains(filePath);
	auto texture = texturePool.GetOrCreate(filePath);
	if (isExists == false) {
		LoadTextureFromFilePath(filePath.GetString(), *texture, textureFormat, channelCount, bindFlag, generateMipmap);
	}
	return texture;
}

template<typename ResourceT>
inline std::shared_ptr<ResourceT> Cjing3D::ResourceManager::GetOrCreateEmptyResource(const StringID& name)
{
	PoolType<ResourceT>& resourcePool = GetPool< ResourceT >();
	return resourcePool.GetOrCreate(name);
}