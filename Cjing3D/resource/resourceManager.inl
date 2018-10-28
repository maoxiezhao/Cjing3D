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
inline const ResourceManager::PoolType<VertexShader>&
ResourceManager::GetPool<VertexShader>()const
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