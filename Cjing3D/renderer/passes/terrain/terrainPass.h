#pragma once

#include "renderer\passes\renderPass.h"
#include "renderer\passes\terrain\terrainTree.h"

namespace Cjing3D
{
	class TerrainPass : public RenderPass
	{
	public:
		TerrainPass();
		virtual ~TerrainPass();

		virtual void Initialize();
		virtual void Uninitialize();
		virtual void UpdatePerFrameData(F32 deltaTime);
		virtual void RefreshRenderData(CommandList cmd);
		virtual void Render(CommandList cmd);

		U32 GetElevation(ECS::Entity entity)const;
		void SetElevation(ECS::Entity entity, U32 elevation);

		TerrainTreePtr RegisterTerrain(ECS::Entity entity, U32 width, U32 height, U32 elevation);
		void UnRegisterTerrain(ECS::Entity entity);
		TerrainTreePtr GetTerrainTree(ECS::Entity entity);

	private:
		void InitializeTestData();

		std::vector<ECS::Entity> mToRemovedTerrainTree;
		std::map<ECS::Entity, TerrainTreePtr> mTerrainTreeMap;
	};
}