#pragma once

#include "core\gameSystem.hpp"
#include "resource\resource.h"
#include "resource\resourcePool.hpp"
#include "renderer\renderer.h"
#include "renderer\RHI\rhiResource.h"
#include "renderer\RHI\device.h"
#include "renderer\RHI\rhiShader.h"

namespace Cjing3D
{
/**
*	\brief ��Դ��������
*/
class ResourceManager : public GameSystem
{
public:
	ResourceManager(GameContext& gameContext) :
		GameSystem(gameContext),
		mVertexShaderPool(),
		mPixelShaderPool() {}
	~ResourceManager() = default;

	virtual void Initialize() {};
	virtual void Uninitialize() {};

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
		using poolType = PersistentResourcePool < std::string, ResourceT>;
	};

	// �����ԴΪ��Shader���ͣ�Texture��Model),��ʹ����ͨ��Դ��
	template< typename ResourceT>
	struct ResourceRecord<ResourceT, std::enable_if_t<!is_shader<ResourceT>::value> >
	{
		using poolType = ResourcePool < std::string, ResourceT>;
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

private:
	template <typename ResourceT>
	PoolType<ResourceT>& GetPool();

	template <typename ResourceT>
	const PoolType<ResourceT>& GetPool()const;


private:
	std::map<Resource_Type, std::string> mResourceDirectories;

	PoolType<VertexShader> mVertexShaderPool;
	PoolType<PixelShader> mPixelShaderPool;
};

#include "resourceManager.inl"


}