#include "mapTileset.h"
#include "game\map\ground\gameMapGroundRenderer.h"

namespace CjingGame
{
	void MapTileset::Bind()
	{
		GameMapGroundRenderer::SetCurrentTileset(&mGroundTileset);
	}
}