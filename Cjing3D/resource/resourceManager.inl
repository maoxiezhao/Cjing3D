
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
inline const ResourceManager::PoolType<Model>&
ResourceManager::GetPool<Model>()const
{
	return mModelPool;
}

template <>
inline const ResourceManager::PoolType<RhiTexture2D>&
ResourceManager::GetPool<RhiTexture2D>()const
{
	return mTexture2DPool;
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
		auto vertexShaderInfo = shaderPool.GetOrCreate(name);
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
			const HRESULT result = device.CreateInputLayout(desc, numElements, static_cast<const void*>(byteData.data()),
				byteData.size(), *vertexShaderInfo->mInputLayout);
			Debug::ThrowIfFailed(result, "Failed to create input layout: %08X", result);
		}
	}

	return vertexShaderInfo;
}

template<typename ResourceT>
inline std::enable_if_t<std::is_same<ResourceT, Model>::value, std::shared_ptr<Model>>
Cjing3D::ResourceManager::GetOrCreate(const StringID & filePath)
{
	PoolType<Model>& modelPool = GetPool< Model >();
	bool isExists = modelPool.Contains(filePath);

	auto model = modelPool.GetOrCreate(filePath);
	if (isExists == false) {
		model->LoadFromFile(filePath.GetString(), *this);
	}
	return model;
}
