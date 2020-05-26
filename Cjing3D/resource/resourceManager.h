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
*	\brief ��Դ��������
*	
*	SUPPORT:
*		Resource_Texture,
*
*  TODO: refactor
// 1. ֧�ֳ־û���texture�����ü�����texture
// 2. ��Դ�ṹ�Ż�
// 3. �첽����֧��
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

	// �����Դ������Shader���ͣ���ʹ�ó־û���Դ��
	template<typename ResourceT>
	struct ResourceRecord<ResourceT, std::enable_if_t<is_shader<ResourceT>::value> >
	{
	public:
		using poolType = PersistentResourcePool < StringID, ResourceT>;
	};

	// �����ԴΪ��Shader���ͣ�Texture),��ʹ����ͨ��Դ��
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