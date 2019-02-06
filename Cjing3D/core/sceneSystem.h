#pragma once

#include "common\common.h"
#include "world\component\transform.h"


#include <map>

namespace Cjing3D {

	// 目前所有的Component放在一起，以后可能会分开

	struct MeshComponent 
	{
	public:
		std::vector<F32x3> mVertexPositions;
		std::vector<F32x3> mVertexNormals;
		std::vector<F32x2> mVertexTexcoords;
		std::vector<U32> mIndices;

		struct MeshSubset
		{
			ECS::Entity mMaterialID = ECS::INVALID_ENTITY;
			U32 mIndexOffset = 0;
			U32 mIndexCount = 0;
		};
		std::vector<MeshSubset> mSubsets;

		std::unique_ptr<GPUBuffer> mVertexBufferPos;
		std::unique_ptr<GPUBuffer> mVertexBufferTex;
		std::unique_ptr<GPUBuffer> mIndexBuffer;

		AABB mAABB;

	public:
		void SetupRenderData(GraphicsDevice& device);

	public:

		// 顶点结构体，因为法线值可以以（0-256）存储,故法线坐标可以用3个8位
		// 表示，同时加上subsetIndex存储在第四个8位中
		struct VertexPosNormalSubset
		{
			F32x3 mPos = F32x3(0.0f, 0.0f, 0.0f);
			//  subsetindex, normal_z, normal_y, normal_x
			U32 mNormalSubsetIndex = 0;

			void Setup(F32x3 pos, F32x3 normal, U32 subsetIndex);
			void Setup(F32x3 normal, U32 subsetIndex);
		};

		// TODO:可以用两个Half float 表示的纹理坐标
		struct VertexTex
		{
			F32x2 mTex = F32x2(0.0f, 0.0f);
			static const FORMAT format = FORMAT::FORMAT_R32G32_FLOAT;
		};
	};

	struct MaterialComponent
	{
	public:
		F32x4 mBaseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		F32 mRoughness = 0.2f;
		F32 mMetalness = 0.0f;

		std::string mBaseColorMapName;
		std::string mSurfaceMapName;
		std::string mNormalMapName;

		RhiTexture2DPtr mBaseColorMap = nullptr;
		RhiTexture2DPtr mSurfaceMap = nullptr;
		RhiTexture2DPtr mNormalMap = nullptr;

		inline bool IsCastingShadow()const { return mIsCastingShadow; }

		bool mIsCastingShadow = false;
	};

	struct ObjectComponent
	{
	public:
		enum ObjectType
		{
			OjbectType_Renderable
		};

		ECS::Entity mMeshID = ECS::INVALID_ENTITY;
		F32x3 mCenter = F32x3(0.0f, 0.0f, 0.0f);
		F32x4 mColor = F32x4(1.0f, 1.0f, 1.0f, 1.0f);
		ObjectType mObjectType = OjbectType_Renderable;

		RenderableType mRenderableType = RenderableType_Opaque;
		bool mIsCastingShadow = false;

		inline ObjectType GetObjectType() { return mObjectType; }
		inline RenderableType GetRenderableType() { return mRenderableType; }
		inline void SetCastingShadow(bool isCastingShadow) { mIsCastingShadow = isCastingShadow; }
	};

	// TODO: refactor with template
	class Scene
	{
	public:
		Scene();

		void Update();
		void Merge(Scene& scene);
		void Clear();

		// create function
		ECS::Entity CreateEntityMaterial(const std::string& name);
		ECS::Entity CreateEntityMesh(const std::string& name);
		ECS::Entity CreateEntityObject(const std::string& name);

		ECS::Entity CreateEntityByName(const std::string& name);
		ECS::Entity GetEntityByName(StringID name);

		void RemoveEntity(ECS::Entity entity);

		template<typename ComponentT>
		std::shared_ptr<ComponentT> GetComponent(ECS::Entity entity)
		{
			auto& manager = GetComponentManager<ComponentT>();
			return manager.GetComponent(entity);
		}

	private:
		template<typename ComponentT>
		ECS::ComponentManager<ComponentT>& GetComponentManager();

		void UpdateSceneObject();

	public:
		std::map<StringID, ECS::Entity> mNameEntityMap;

		ECS::ComponentManager<StringID> mNames;
		ECS::ComponentManager<MeshComponent> mMeshes;
		ECS::ComponentManager<MaterialComponent> mMaterials;
		ECS::ComponentManager<ObjectComponent> mObjects;
		ECS::ComponentManager<AABB> mObjectAABBs;
		ECS::ComponentManager<TransformComponent> mTransforms;

		AABB mSceneAABB;
	};

	template<>
	inline ECS::ComponentManager<MeshComponent>& Scene::GetComponentManager()
	{
		return mMeshes;
	}

	template<>
	inline ECS::ComponentManager<MaterialComponent>& Scene::GetComponentManager()
	{
		return mMaterials;
	}

	template<>
	inline ECS::ComponentManager<ObjectComponent>& Scene::GetComponentManager()
	{
		return mObjects;
	}

	template<>
	inline ECS::ComponentManager<AABB>& Scene::GetComponentManager()
	{
		return mObjectAABBs;
	}

	template<>
	inline ECS::ComponentManager<TransformComponent>& Scene::GetComponentManager()
	{
		return mTransforms;
	}
}
	