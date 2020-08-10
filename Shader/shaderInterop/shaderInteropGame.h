#ifndef _SHADERINTEROP_GAME_H_
#define _SHADERINTEROP_GAME_H_

#include "shaderInterop.h"

CBUFFER(GroundTileSetCB, CBSLOT_GAME_GROUND_TILESET)
{
	uint gGroundTilesetWidth;
	uint gGroundTilesetHeight;
	float gGroundTilesetInvWidth;
	float gGroundTilesetInvHeight;
};


#endif