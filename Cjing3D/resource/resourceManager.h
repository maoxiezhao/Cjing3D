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
*		Resrouce_VertexShader,
*		Resrouce_PixelShader,
*		Resource_Texture,
*/
class ResourceManager : public SubSystem
{
public:
	ResourceManager(SystemContext& gameContext) :
		SubSystem(gameContext),
		mVertexShaderPool(),
		mPixelShaderPool() {}
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

	template <typename ResourceT>
	std::enable_if_t<std::is_same<ResourceT, VertexShaderInfo>::value, std::shared_ptr<VertexShaderInfo> >
		GetOrCreate(const StringID& name, VertexLayoutDesc* desc, U32 numElements);

	template <typename ResourceT>
	std::enable_if_t<std::is_same<ResourceT, PixelShader>::value, std::shared_ptr<PixelShader> >
		GetOrCreate(const StringID& name);

	template <typename ResourceT>
	std::enable_if_t<std::is_same<ResourceT, ComputeShader>::value, std::shared_ptr<ComputeShader> >
		GetOrCreate(const StringID& name);

	template<typename ResourceT>
	std::enable_if_t<std::is_same<ResourceT, RhiTexture2D>::value, std::shared_ptr<RhiTexture2D>>
		GetOrCreate(const StringID & filePath);

private:
	template <typename ResourceT>
	PoolType<ResourceT>& GetPool();

	template <typename ResourceT>
	const PoolType<ResourceT>& GetPool()const;

	void LoadTextrueFromFilePath(const std::filesystem::path& filePath, RhiTexture2D& texture);

private:
	std::map<Resource_Type, std::string> mResourceDirectories;

	PoolType<VertexShaderInfo> mVertexShaderPool;
	PoolType<PixelShader> mPixelShaderPool;
	PoolType<ComputeShader> mComputeShaderPool;
	PoolType<RhiTexture2D> mTexture2DPool;
};

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


#include "resource\resourceManager.inl"

}