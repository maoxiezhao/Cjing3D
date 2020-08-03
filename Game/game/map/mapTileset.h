#pragma once

#include "game\map\gameMapGroundTileset.h"

namespace CjingGame
{
	struct MapTileset
	{
		GameMapGroundTileset mGroundTileset;

		void Bind();
	};
}