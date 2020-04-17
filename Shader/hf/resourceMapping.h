#ifndef _RESOURCE_DEFINE_H_
#define _RESOURCE_DEFINE_H_

// (register t##slot) ///////////////////////////////////////////////////////////
// TEXTURE SLOT 
#define TEXTURE_SLOT_SHADOW_ARRAY_2D 0

#define TEXTURE_SLOT_0 1
#define TEXTURE_SLOT_1 2
#define TEXTURE_SLOT_2 3
#define TEXTURE_SLOT_3 4
#define TEXTURE_SLOT_4 5
#define TEXTURE_SLOT_5 6

#define TEXTURE_SLOT_UNIQUE_0 7
#define TEXTURE_SLOT_UNIQUE_1 8

// STRUCTUREDBUFFRE SLOT
#define SBSLOT_SHADER_LIGHT_ARRAY 9
#define SBSLOT_MATRIX_ARRAY 10

#define SHADER_MATRIX_COUNT 128
#define SHADER_LIGHT_COUNT 128

///////////////////////////////////////////////////////////////////////////////////

// SAMPLER SLOT (register s##slot)
#define SAMPLER_SLOT_0 0
#define SAMPLER_SLOT_1 1
#define SAMPLER_COMPARISON_SLOT 2

#define SAMPLER_LINEAR_CLAMP_SLOT SAMPLER_SLOT_0
#define SAMPLER_ANISOTROPIC_SLOT  SAMPLER_SLOT_1

///////////////////////////////////////////////////////////////////////////////////

// skinning
#define SKINNING_SLOT_VERTEX_POS 0
#define SKINNING_SLOT_VERTEX_BONE_INDEX_WEIGHT 1
#define SKINNING_SBSLOT_BONE_POSE_BUFFER 2

#endif