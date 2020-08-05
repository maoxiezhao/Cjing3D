#pragma once

#include "definitions\definitions.h"
#include "system\sceneSystem.h"
#include "game\map\ground\gameMapGroundTileset.h"
#include "game\map\ground\gameMapGround.h"

namespace CjingGame
{
	struct GameMapGroundInst
	{
		ECS::Entity mMaterialEntity = INVALID_ENTITY;
		ECS::Entity mMeshEntity = INVALID_ENTITY;
		ECS::Entity mObjectEntity = INVALID_ENTITY;

		bool IsValid()const;
		void Clear();
	};
	using GameMapGroundInstPtr = std::shared_ptr<GameMapGroundInst>;

	namespace GameMapGroundRenderer
	{
		void Initialize();
		void Uninitilize();
		void PreRender();

		GameMapGroundInstPtr CreateStaticInst();
		void RemoveStaticInst(GameMapGroundInstPtr inst);
		void GenerateMesh(ECS::Entity meshEntity, std::map<I32x3, std::shared_ptr<GameMapGround>>& grounds);
		void SetCurrentTileset(GameMapGroundTileset* tileset);	
	};
}