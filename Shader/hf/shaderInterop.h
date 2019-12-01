#ifndef _SHADERINTEROP_H_
#define _SHADERINTEROP_H_

#include "shaderDefine.h"

#ifdef __cplusplus

#include <DirectXMath.h>

using namespace DirectX;

typedef XMMATRIX matrix;
typedef XMFLOAT4X4 float4x4;
typedef XMFLOAT2 float2;
typedef XMFLOAT3 float3;
typedef XMFLOAT4 float4;
typedef uint32_t uint;
typedef XMUINT2 uint2;
typedef XMUINT3 uint3;
typedef XMUINT4 uint4;
typedef XMINT2 int2;
typedef XMINT3 int3;
typedef XMINT4 int4;

#define CB_GETSLOT_NAME(name) __CBUFFER_SLOT_NAME_##name##__
#define CBUFFER(name, slot) static const int CB_GETSLOT_NAME(name) = slot; struct alignas(16) name

#else

#define CBUFFER(name, slot) cbuffer name : register(b ## slot)
#define TEXTURE2D(name, slot) Texture2D name : register(t ## slot);
#define SAMPLERSTATE(name, slot) SamplerState name :register(s ## slot);
#define STRUCTREDBUFFER(name, type, slot) StructuredBuffer<type> name :register(t ## slot);

#endif

#endif