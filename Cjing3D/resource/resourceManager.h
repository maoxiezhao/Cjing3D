#pragma once

#include "core\subSystem.hpp"
#include "helper\fileSystem.h"
#include "resource\resource.h"
#include "resource\resourcePool.hpp"
#include "resource\modelImporter.h"
#include "renderer\renderer.h"
#include "renderer\RHI\rhiResource.h"
#include "renderer\RHI\device.h"
#include "renderer\RHI\rhiShader.h"
#include "core\systemContext.hpp"

#include <filesystem>

namespace Cjing3D
{
/**
*	\brief 资源管理器类
*	
*	SUPPORT:
*		Resource_Texture,
*
*  TODO: refactor
// 1. 支持持久化的texture和引用计数的texture
// 2. 资源结构优化
// 3. 异步加载支持
*/
class ResourceManager : public SubSystem
{
public:
	ResourceManager(SystemContext& gameContext) :
		SubSystem(gameContext) {}
	~ResourceManager() = default;

	void Initialize();
	void Uninitialize();
	
	void AddStandardResourceDirectory(Resource_Type type, const std::string& path);
	const std::string& GetStandardResourceDirectory(Resource_Type type)const;

	//----------------------------------------------------------------
	//	Common Method
	//----------------------------------------------------------------

	template< typename ResourceT, typename Enable = void>
	struct ResourceRecord;

	// 如果资源类型是Shader类型，则使用持久化资源池
	template<typename ResourceT>
	struct ResourceRecord<ResourceT, std::enable_if_t<is_shader<ResourceT>::value> >
	{
	public:
		using poolType = PersistentResourcePool < StringID, ResourceT>;
	};

	// 如果资源为非Shader类型（Texture),则使用普通资源池
	template< typename ResourceT>
	struct ResourceRecord<ResourceT, std::enable_if_t<!is_shader<ResourceT>::value> >
	{
		using poolType = ResourcePool < StringID, ResourceT>;
	};

	// 获取指定资源的Key类型和Resource类型
	template <typename ResourceT>
	using PoolType = typename ResourceRecord<ResourceT>::poolType;
	template <typename ResourceT>
	using KeyType = typename PoolType<ResourceT>::KeyType;
	template <typename ResourceT>
	using ResourceType = typename PoolType<ResourceT>::ResourceType;

	template <typename ResourceT>
	std::shared_ptr<typename ResourceType<ResourceT> > Get(const typename KeyType<ResourceT>& guid);

	template <typename ResourceT>
	bool Contains(const typename KeyType<ResourceT>& guid)const;

	//----------------------------------------------------------------
	//	Special Method
	//----------------------------------------------------------------

	ResourcePtr GetOrCreateByType(const StringID& name, Resource_Type type);

	template<typename ResourceT>
	std::shared_ptr< ResourceT> GetOrCreateEmptyResource(const StringID& name);

	template<typename ResourceT>
	std::enable_if_t<std::is_same<ResourceT, RhiTexture2D>::value, std::shared_ptr<RhiTexture2D>>
		GetOrCreate(const StringID & filePath);

	template<typename ResourceT>
	std::enable_if_t<std::is_same<ResourceT, RhiTexture2D>::value, std::shared_ptr<RhiTexture2D>>
		GetOrCreate(const StringID& filePath, FORMAT textureFormat, U32 channelCount = 4, U32 bindFlag = BIND_SHADER_RESOURCE, bool generateMipmap = false);

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

	void LoadTextrueFromFilePath(const std::filesystem::path& filePath, RhiTexture2D& texture);
	void LoadTextureFromFilePathEx(const std::filesystem::path& filePath, RhiTexture2D& texture, FORMAT textureFormat = FORMAT_R8G8B8A8_UNORM, U32 channelCount = 4, U32 bindFlag = BIND_SHADER_RESOURCE, bool generateMipmap = false);

private:
	template <typename ResourceT>
	PoolType<ResourceT>& GetPool();

	template <typename ResourceT>
	const PoolType<ResourceT>& GetPool()const;

private:
	std::map<Resource_Type, std::string> mResourceDirectories;

	PoolType<RhiTexture2D> mTexture2DPool;
};

#include "resource\resourceManager.inl"

}