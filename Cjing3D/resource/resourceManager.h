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
#include "renderer\components\model.h"
#include "core\systemContext.hpp"

namespace Cjing3D
{

/**
*	\brief ��Դ��������
*/
class ResourceManager : public SubSystem
{
public:
	ResourceManager(SystemContext& gameContext) :
		SubSystem(gameContext),
		mVertexShaderPool(),
		mPixelShaderPool() {}
	~ResourceManager() = default;

	virtual void Initialize();
	virtual void Uninitialize() {};
	
	void AddStandardResourceDirectory(Resource_Type type, const std::string& path);
	const std::string& GetStandardResourceDirectory(Resource_Type type)const;

	ModelImporter& GetModelImporter();

	//----------------------------------------------------------------
	//	Common Method
	//----------------------------------------------------------------

	template< typename ResourceT, typename Enable = void>
	struct ResourceRecord;

	// �����Դ������Shader���ͣ���ʹ�ó־û���Դ��
	template<typename ResourceT>
	struct ResourceRecord<ResourceT, std::enable_if_t<is_shader<ResourceT>::value> >
	{
	public:
		using poolType = PersistentResourcePool < StringID, ResourceT>;
	};

	// �����ԴΪ��Shader���ͣ�Texture��Model),��ʹ����ͨ��Դ��
	template< typename ResourceT>
	struct ResourceRecord<ResourceT, std::enable_if_t<!is_shader<ResourceT>::value> >
	{
		using poolType = ResourcePool < StringID, ResourceT>;
	};

	// ��ȡָ����Դ��Key���ͺ�Resource����
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

	template<typename ResourceT>
	std::enable_if_t<std::is_same<ResourceT, Model>::value, std::shared_ptr<Model>>
		GetOrCreate(const StringID & filePath);

	template<typename ResourceT>
	std::enable_if_t<std::is_same<ResourceT, RhiTexture2D>::value, std::shared_ptr<RhiTexture2D>>
		GetOrCreate(const StringID & filePath, GraphicsDevice& device);

private:
	template <typename ResourceT>
	PoolType<ResourceT>& GetPool();

	template <typename ResourceT>
	const PoolType<ResourceT>& GetPool()const;

	std::unique_ptr<ModelImporter> mModelImporter;

private:
	std::map<Resource_Type, std::string> mResourceDirectories;

	PoolType<VertexShaderInfo> mVertexShaderPool;
	PoolType<PixelShader> mPixelShaderPool;
	PoolType<RhiTexture2D> mTexture2DPool;

	// TO REMOVE
	PoolType<Model> mModelPool;
	PoolType<Mesh> mMeshPool;
};

template<typename ResourceT>
inline std::enable_if_t<std::is_same<ResourceT, RhiTexture2D>::value, std::shared_ptr<RhiTexture2D>>
Cjing3D::ResourceManager::GetOrCreate(const StringID & filePath, GraphicsDevice& device)
{
	PoolType<RhiTexture2D>& texturePool = GetPool< RhiTexture2D >();

	bool isExists = texturePool.Contains(filePath);
	auto texture = texturePool.GetOrCreate(filePath, device);
	if (isExists == false) {
		// load Image
	}
	return texture;
}

#include "resource\resourceManager.inl"


}