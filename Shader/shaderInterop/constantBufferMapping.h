#ifndef _CONSTANT_BUFFER_DEFINE_H_
#define _CONSTANT_BUFFER_DEFINE_H_


// CBUFFER SLOT (register b##slot)
#define CBSLOT_RENDERER_COMMON      0
#define CBSLOT_RENDERER_FRAME       1
#define CBSLOT_RENDERER_CAMERA      2
#define CBSLOT_IMAGE				3
#define CBSLOT_FONT                 4
#define CBSLOT_RENDERER_MATERIAL    5
#define CBSLOT_MISC                 6

#define CBSLOT_POSTPROCESS			7
#define CBSLOT_MIPMAPGENERATE		7
#define CBSLOT_TERRAIN				7
#define CBSLOT_CUBEMAP              7
#define CBSLOT_CS_PARAMS            7
#define CBSLOT_SPRITE               7
#define CBSLOT_PARTICLE             7
#define CBSLOT_IMGUI                7

// CBUFFER Game Slot
#define CBSLOT_GAME_GROUND_TILESET  8

#endif