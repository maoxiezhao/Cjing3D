#pragma once

#include "game\map\ground\gameMapGroundTileset.h"

namespace CjingGame
{
	struct MapTileset
	{
		GameMapGroundTileset mGroundTileset;

		void Bind();
	};
}