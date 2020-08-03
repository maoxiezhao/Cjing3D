#include "gameMapRenderer.h"
#include "game\map\gameMapGroundRenderer.h"

namespace CjingGame {

	void GameMapRenderer::Initialize()
	{
		GameMapGroundRenderer::Initialize();
	}

	void GameMapRenderer::Uninitialize()
	{
		GameMapGroundRenderer::Uninitilize();
	}

	void GameMapRenderer::PreRender()
	{
		GameMapGroundRenderer::PreRender();
	}
}