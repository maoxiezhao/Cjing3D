#pragma once

#include "common\common.h"
#include "core\ecsSystem.h"
#include "renderer\renderableCommon.h"
#include "renderer\RHI\rhiDefinition.h"
#include "renderer\RHI\rhiBuffer.h"
#include "helper\stringID.h"

#include <map>

namespace Cjing3D {

	// 目前所有的Component放在一起，以后可能会分开

	struct MeshComponent 
	{
	public:
		std::vector<XMFLOAT3> mVertexPositions;
		std::vector<XMFLOAT3> mVertexNormals;
		std::vector<XMFLOAT2> mVertexTexcoords;
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

	};

	struct MaterialComponent
	{
		RhiTexture2DPtr mBaseColorMap = nullptr;
		RhiTexture2DPtr mSurfaceMap = nullptr;
		RhiTexture2DPtr mNormalMap = nullptr;
	};

	// TODO: refactor with template
	class Scene
	{
	public:
		Scene();

		void Update();
		void Merge(Scene& scene);
		void Clera();

		// create function
		ECS::Entity CreateEntityMaterial(const std::string& name);
		ECS::Entity CreateEntityMesh(const std::string& name);

		ECS::Entity CreateEntityByName(const std::string& name);
		ECS::Entity GetEntityByName(StringID name);

		void RemoveEntity(ECS::Entity entity);

	public:
		std::map<StringID, ECS::Entity> mNameEntityMap;

		ECS::ComponentManager<StringID> mNames;
		ECS::ComponentManager<MeshComponent> mMeshes;
		ECS::ComponentManager<MaterialComponent> mMaterials;
	};

}
	