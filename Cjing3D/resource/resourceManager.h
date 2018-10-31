#pragma once

#include "core\gameSystem.hpp"
#include "helper\fileSystem.h"
#include "resource\resource.h"
#include "resource\resourcePool.hpp"
#include "renderer\renderer.h"
#include "renderer\RHI\rhiResource.h"
#include "renderer\RHI\device.h"
#include "renderer\RHI\rhiShader.h"
#include "core\gameContext.hpp"

namespace Cjing3D
{

/**
*	\brief 资源管理器类
*/
class ResourceManager : public GameSystem
{
public:
	ResourceManager(GameContext& gameContext) :
		GameSystem(gameContext),
		mVertexShaderPool(),
		mPixelShaderPool() {}
	~ResourceManager() = default;

	virtual void Initialize();
	virtual void Uninitialize() {};
	
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

	// 如果资源为非Shader类型（Texture、Model),则使用普通资源池
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

private:
	template <typename ResourceT>
	PoolType<ResourceT>& GetPool();

	template <typename ResourceT>
	const PoolType<ResourceT>& GetPool()const;


private:
	std::map<Resource_Type, std::string> mResourceDirectories;

	PoolType<VertexShaderInfo> mVertexShaderPool;
	PoolType<PixelShader> mPixelShaderPool;
};

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


#include "resourceManager.inl"


}