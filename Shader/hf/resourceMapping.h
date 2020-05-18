#ifndef _RESOURCE_DEFINE_H_
#define _RESOURCE_DEFINE_H_

// (register t##slot) ///////////////////////////////////////////////////////////
// TEXTURE SLOT 
#define TEXTURE_SLOT_GLOBAL_ENV_MAP 0
#define TEXTURE_SLOT_SHADOW_ARRAY_2D 1

#define TEXTURE_SLOT_0 10
#define TEXTURE_SLOT_1 11
#define TEXTURE_SLOT_2 12
#define TEXTURE_SLOT_3 13
#define TEXTURE_SLOT_4 14
#define TEXTURE_SLOT_5 15

#define TEXTURE_SLOT_UNIQUE_0 16
#define TEXTURE_SLOT_UNIQUE_1 17

// STRUCTUREDBUFFRE SLOT
#define SBSLOT_SHADER_LIGHT_ARRAY 18
#define SBSLOT_MATRIX_ARRAY 19

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