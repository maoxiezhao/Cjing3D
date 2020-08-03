#include "system\sceneSystem.h"
#include "renderer\passes\terrainPass.h"

namespace Cjing3D
{
	void SceneSystem::UpdateSceneTerrainSystem(Scene& scene)
	{
		ECS::ComponentManager<TerrainComponent >& terrains = scene.mTerrains;
		ECS::ComponentManager<TransformComponent>& transforms = scene.mTransforms;

		for (size_t i = 0; i < terrains.GetCount(); i++)
		{
			Entity entity = terrains.GetEntityByIndex(i);
			TerrainComponent* terrain = terrains[i];
			TransformComponent* transform = transforms.GetComponent(entity);

			if (terrain == nullptr || transform == nullptr) {
				continue;
			}

			U32x2 terrainSize = terrain->GetTerrainSize();
			U32 elevation = terrain->GetElevation();

			// check is registered
			TerrainTreePtr terrainTree = Renderer::GetTerrainTree(entity);
			if (terrainTree == nullptr) {
				terrainTree = Renderer::RegisterTerrain(entity, terrainSize[0], terrainSize[1], elevation);
			}
			else
			{
				if (terrain->IsTerrainDirty())
				{
					terrain->SetTerrainDirty(false);

					if (terrainTree->GetElevation() != elevation) {
						terrainTree->SetElevation(elevation);
					}
					if (terrainTree->GetHeightMap() != terrain->GetHeightMap()) {
						terrainTree->LoadHeightMap(terrain->GetHeightMap());
					}

					terrainTree->LoadTerrainMaterial(terrain->GetTextureMaterial());
				}
			}
		}
	}
}