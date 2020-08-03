#include "mapTileset.h"
#include "gameMapGroundRenderer.h"

namespace CjingGame
{
	void MapTileset::Bind()
	{
		GameMapGroundRenderer::SetCurrentTileset(&mGroundTileset);
	}
}