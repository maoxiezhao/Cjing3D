#include "resourceManager.h"
#pragma once
/**
*	\brief ��ȡָ����Դ
*	\return ����������򷵻�nullptr
*/
template<typename ResourceT>
inline std::shared_ptr<typename ResourceManager::ResourceType<ResourceT> >
ResourceManager::Get(const KeyType<ResourceT>& guid)
{
	auto& shaderPool = GetPool< ResourceT >();
	return shaderPool.Get(guid);
}

/**
*	\brief �ж��Ƿ����ָ����Դ
*/
template<typename ResourceT>
inline bool ResourceManager::Contains(const typename KeyType<ResourceT>& guid) const
{
	auto& shaderPool = GetPool< ResourceT >();
	return shaderPool.Contains(guid);
}

/**
*	\brief ����ָ����Դ��
*/
template<typename ResourceT>
inline ResourceManager::PoolType<ResourceT>& ResourceManager::GetPool()
{
	return const_cast<PoolType<ResourceT>&>(
		static_cast<const ResourceManager*>(this)->GetPool<ResourceT>());
}

/**
*	\brief ����VertexShader Pool
*/
template <>
inline const ResourceManager::PoolType<VertexShaderInfo>&
ResourceManager::GetPool<VertexShaderInfo>()const
{
	return mVertexShaderPool;
}

/**
*	\brief ����PixelShader Pool
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

// ����VertexShader��ɫ��
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

		auto& renderer = mGameContext.GetGameSystem<Renderer>();
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
