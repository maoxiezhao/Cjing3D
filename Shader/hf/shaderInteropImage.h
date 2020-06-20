#ifndef _SHADERINTEROP_IMAGE_H_
#define _SHADERINTEROP_IMAGE_H_

#include "shaderInterop.h"

CBUFFER(ImageCB, CBSLOT_IMAGE)
{
	float4 gImageCorners[4];
	float4 gImageColor;
};

CBUFFER(SpriteCB, CBSLOT_SPRITE)
{
	float4x4 gSpriteCameraVP;
};

#endif