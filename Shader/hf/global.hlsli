#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "shaderInterop.h"
#include "shaderInteropRender.h"

// common texture slots
TEXTURE2D(texture_0, TEXTURE_SLOT_0);
TEXTURE2D(texture_1, TEXTURE_SLOT_1);
TEXTURE2D(texture_2, TEXTURE_SLOT_2);

// common sampler states
SAMPLERSTATE(sampler_linear_clamp, SAMPLER_LINEAR_CLAMP_SLOT);

// common structred buffer
STRUCTREDBUFFER(LightArray, ShaderLight, LIGHTARRAY_SLOT);

#endif