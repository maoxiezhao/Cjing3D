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

/**
*	\brief 返回VertexShader Pool
*/
template <>
inline const ResourceManager::PoolType<VertexShaderInfo>&
ResourceManager::GetPool<VertexShaderInfo>()const
{
	return mVertexShaderPool;
}

/**
*	\brief 返回PixelShader Pool
*/
template <>
inline const ResourceManager::PoolType<PixelShader>&
ResourceManager::GetPool<PixelShader>()const
{
	return mPixelShaderPool;
}

template <>
inline const ResourceManager::PoolType<RhiTexture2D>&
ResourceManager::GetPool<RhiTexture2D>()const
{
	return mTexture2DPool;
}

template <>
inline const ResourceManager::PoolType<ComputeShader>&
ResourceManager::GetPool<ComputeShader>()const
{
	return mComputeShaderPool;
}

template <>
inline const ResourceManager::PoolType<HullShader>&
ResourceManager::GetPool<HullShader>()const
{
	return mHullShaderPool;
}

template <>
inline const ResourceManager::PoolType<DomainShader>&
ResourceManager::GetPool<DomainShader>()const
{
	return mDomainShaderPool;
}



// 创建VertexShader着色器
template<typename ResourceT>
inline std::enable_if_t<std::is_same<ResourceT, VertexShaderInfo>::value, std::shared_ptr<VertexShaderInfo>>
Cjing3D::ResourceManager::GetOrCreate(const StringID & name, VertexLayoutDesc* desc, U32 numElements)
{
	std::shared_ptr<VertexShaderInfo> vertexShaderInfo = nullptr;
	const std::string byteData = FileData::ReadFile(name.GetString());
	if (byteData.empty() == false)
	{
		PoolType<VertexShaderInfo>& shaderPool = GetPool< VertexShaderInfo >();
		vertexShaderInfo = shaderPool.GetOrCreate(name);
		vertexShaderInfo->mVertexShader = std::make_shared<VertexShader>();
		vertexShaderInfo->mInputLayout = std::make_shared<InputLayout>();

		auto& renderer = mGameContext.GetSubSystem<Renderer>();
		auto& device = renderer.GetDevice();
		{
			const HRESULT result = device.CreateVertexShader(static_cast<const void*>(byteData.data()), byteData.size(),
				*vertexShaderInfo->mVertexShader);
			Debug::ThrowIfFailed(result, "Failed to create vertex shader: %08X", result);
		}
		{
			if (desc != nullptr)
			{
				const HRESULT result = device.CreateInputLayout(desc, numElements, static_cast<const void*>(byteData.data()),
					byteData.size(), *vertexShaderInfo->mInputLayout);
				Debug::ThrowIfFailed(result, "Failed to create input layout: %08X", result);
			}
		}
	}

	return vertexShaderInfo;
}



template <typename ResourceT>
inline std::enable_if_t<std::is_same<ResourceT, PixelShader>::value, std::shared_ptr<PixelShader> >
ResourceManager::GetOrCreate(const StringID& name)
{
	std::shared_ptr<PixelShader> pixelShader = nullptr;
	const std::string byteData = FileData::ReadFile(name.GetString());
	if (byteData.empty() == false)
	{
		PoolType<PixelShader>& shaderPool = GetPool< PixelShader >();
		pixelShader = shaderPool.GetOrCreate(name);

		auto& renderer = mGameContext.GetSubSystem<Renderer>();
		auto& device = renderer.GetDevice();
		{
			const HRESULT result = device.CreatePixelShader(static_cast<const void*>(
				byteData.data()), byteData.size(), *pixelShader);
			Debug::ThrowIfFailed(result, "Failed to create pixel shader: %08X", result);
		}
	}

	return pixelShader;
}

template<typename ResourceT>
inline std::enable_if_t<std::is_same<ResourceT, RhiTexture2D>::value, std::shared_ptr<RhiTexture2D>>
ResourceManager::GetOrCreate(const StringID & filePath)
{
	PoolType<RhiTexture2D>& texturePool = GetPool< RhiTexture2D >();

	bool isExists = texturePool.Contains(filePath);
	auto texture = texturePool.GetOrCreate(filePath);
	if (isExists == false) {
		LoadTextureFromFilePathEx(filePath.GetString(), *texture);
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
		LoadTextureFromFilePathEx(filePath.GetString(), *texture, textureFormat, channelCount, bindFlag, generateMipmap);
	}
	return texture;
}

template <typename ResourceT>
inline std::enable_if_t<std::is_same<ResourceT, ComputeShader>::value, std::shared_ptr<ComputeShader> >
ResourceManager::GetOrCreate(const StringID& name)
{
	std::shared_ptr<ComputeShader> computeShader = nullptr;
	const std::string byteData = FileData::ReadFile(name.GetString());
	if (byteData.empty() == false)
	{
		PoolType<ComputeShader>& shaderPool = GetPool< ComputeShader >();
		computeShader = shaderPool.GetOrCreate(name);

		auto& renderer = mGameContext.GetSubSystem<Renderer>();
		auto& device = renderer.GetDevice();
		{
			const HRESULT result = device.CreateComputeShader(static_cast<const void*>(
				byteData.data()), byteData.size(), *computeShader);
			Debug::ThrowIfFailed(result, "Failed to create compute shader: %08X", result);
		}
	}

	return computeShader;
}

template <typename ResourceT>
inline std::enable_if_t<std::is_same<ResourceT, HullShader>::value, std::shared_ptr<HullShader> >
ResourceManager::GetOrCreate(const StringID& name)
{
	std::shared_ptr<HullShader> hullShader = nullptr;
	const std::string byteData = FileData::ReadFile(name.GetString());
	if (byteData.empty() == false)
	{
		PoolType<HullShader>& shaderPool = GetPool< HullShader >();
		hullShader = shaderPool.GetOrCreate(name);

		auto& renderer = mGameContext.GetSubSystem<Renderer>();
		auto& device = renderer.GetDevice();
		{
			const HRESULT result = device.CreateHullShader(static_cast<const void*>(
				byteData.data()), byteData.size(), *hullShader);
			Debug::ThrowIfFailed(result, "Failed to create hull shader: %08X", result);
		}
	}

	return hullShader;
}

template <typename ResourceT>
inline std::enable_if_t<std::is_same<ResourceT, DomainShader>::value, std::shared_ptr<DomainShader> >
ResourceManager::GetOrCreate(const StringID& name)
{
	std::shared_ptr<DomainShader> domainShader = nullptr;
	const std::string byteData = FileData::ReadFile(name.GetString());
	if (byteData.empty() == false)
	{
		PoolType<DomainShader>& shaderPool = GetPool< DomainShader >();
		domainShader = shaderPool.GetOrCreate(name);

		auto& renderer = mGameContext.GetSubSystem<Renderer>();
		auto& device = renderer.GetDevice();
		{
			const HRESULT result = device.CreateDomainShader(static_cast<const void*>(
				byteData.data()), byteData.size(), *domainShader);
			Debug::ThrowIfFailed(result, "Failed to create domain shader: %08X", result);
		}
	}

	return domainShader;
}

template<typename ResourceT>
inline std::shared_ptr<ResourceT> Cjing3D::ResourceManager::GetOrCreateEmptyResource(const StringID& name)
{
	PoolType<ResourceT>& resourcePool = GetPool< ResourceT >();
	return resourcePool.GetOrCreate(name);
}