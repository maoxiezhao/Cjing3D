#include "resourceManager.h"

template <>
inline const ResourceManager::PoolType<TextureResource>&
ResourceManager::GetPool<TextureResource>()const
{
	return mTexture2DPool;
}
template <>
inline const ResourceManager::PoolType<SoundResource>&
ResourceManager::GetPool<SoundResource>()const
{
	return mSoundPool;
}

template<typename ResourceT>
inline std::enable_if_t<std::is_same<ResourceT, TextureResource>::value, std::shared_ptr<TextureResource>>
ResourceManager::GetOrCreate(const StringID& filePath, FORMAT textureFormat, U32 channelCount, U32 bindFlag, bool generateMipmap)
{
	PoolType<TextureResource>& texturePool = GetPool< TextureResource >();

	bool isExists = texturePool.Contains(filePath);
	auto texture = texturePool.GetOrCreate(filePath);
	if (isExists == false) {
		LoadTextureFromFilePath(filePath.GetString(), *texture->mTexture, textureFormat, channelCount, bindFlag, generateMipmap);
	}
	return texture;
}

template<typename ResourceT>
inline std::enable_if_t<std::is_same<ResourceT, SoundResource>::value, std::shared_ptr<SoundResource>>
ResourceManager::GetOrCreate(const StringID& filePath)
{
	PoolType<SoundResource>& soundPool = GetPool< SoundResource >();

	bool isExists = soundPool.Contains(filePath);
	auto sound = soundPool.GetOrCreate(filePath);
	if (isExists == false) {
		LoadSoundFromFilePath(filePath.GetString(), *sound);
	}
	return sound;
}

template<typename ResourceT>
inline std::shared_ptr<ResourceT> Cjing3D::ResourceManager::GetOrCreateEmptyResource(const StringID& name)
{
	PoolType<ResourceT>& resourcePool = GetPool< ResourceT >();
	return resourcePool.GetOrCreate(name);
}