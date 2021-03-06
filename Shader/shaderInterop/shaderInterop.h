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
// buffer
#define CBUFFER(name, slot) cbuffer name : register(b ## slot)
#define RAWBUFFER(name,slot) ByteAddressBuffer name : register(t ## slot)
#define RWRAWBUFFER(name,slot) RWByteAddressBuffer name : register(u ## slot)
#define TYPEDBUFFER(name, type, slot) Buffer<type> name : register(t ## slot)

// texture
#define TEXTURE2D(name, type, slot) Texture2D<type> name : register(t ## slot);
#define TEXTURE2DARRAY(name, type, slot) Texture2DArray<type> name : register(t ## slot);
#define TEXTRRECUBE(name, type, slot)  TextureCube<type> name : register(t ## slot);
#define TEXTRRECUBEARRAY(name, type, slot)  TextureCubeArray<type> name : register(t ## slot);

// sampler
#define SAMPLERSTATE(name, slot) SamplerState name :register(s ## slot);
#define SAMPLERCOMPARISONSTATE(name, slot) SamplerComparisonState name :register(s ## slot);
#define STRUCTUREDBUFFER(name, type, slot) StructuredBuffer<type> name :register(t ## slot);

// unordered access
#define RWTEXTURE2D(name, type, slot) RWTexture2D<type> name : register(u ## slot);
#define RWTEXTURE2DArray(name, type, slot) RWTexture2DArray<type> name : register(u ## slot);
#define RWSTRUCTUREDBUFFER(name, type, slot) RWStructuredBuffer<type> name :register(u ## slot);

#endif
#endif