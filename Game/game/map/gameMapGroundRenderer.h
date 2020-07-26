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

		std::shared_ptr<GameMapGround> CreateGroundObject(Scene& scene);

	private:
		void InitializeRender();
		void InitializeMaterial();
		void InitializeMesh();
		
		ECS::Entity mMaterialEntity = INVALID_ENTITY;
		ECS::Entity mMeshEntity = INVALID_ENTITY;

		GameMapGroundTileset* mGameMapGroundTileset = nullptr;
	};
}