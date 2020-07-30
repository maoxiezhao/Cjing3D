#pragma once

#include "definitions\definitions.h"
#include "system\sceneSystem.h"
#include "game\map\gameMapGroundTileset.h"
#include "game\map\gameMapGround.h"

namespace CjingGame
{
	class GameMapGroundRenderer
	{
	public:
		GameMapGroundRenderer();
		~GameMapGroundRenderer();

		void Initialize();
		void Uninitilize();
		void SetCurrentTileset(GameMapGroundTileset* tileset);
		void BindPreRender();
		void GenerateMesh(std::map<I32x3, std::shared_ptr<GameMapGround>>& grounds);

		std::shared_ptr<GameMapDynamicGround> CreateDynamicGround(Scene& scene);

	private:
		void InitializeRender();
		void InitializeMaterial();
		void InitializeMesh();
		void InitializeObject();
		
		// static
		ECS::Entity mMaterialEntity = INVALID_ENTITY;
		ECS::Entity mMeshEntity = INVALID_ENTITY;
		ECS::Entity mObjectEntity = INVALID_ENTITY;

		// dynamic
		ECS::Entity mDynamicMeshEntity = INVALID_ENTITY;
		ECS::Entity mDynamicMaterialEntity = INVALID_ENTITY;

		GameMapGroundTileset* mGameMapGroundTileset = nullptr;
	};
}