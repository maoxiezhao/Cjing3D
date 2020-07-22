#include "..\hf\global.hlsli"

#ifndef TARGET_FORMAT
#define TARGET_FORMAT float4
#endif

TEXTRRECUBE(texture_input, float4, TEXTURE_SLOT_UNIQUE_0);
RWTEXTURE2DArray(texture_output, TARGET_FORMAT, 0);
SAMPLERSTATE(genSampler, SAMPLER_SLOT_0);

inline float3 GetCubeMapDir(in float2 uv, in uint faceIndex)
{
    // convert to [-1, 1]
    uv = uv * 2 - 1;
    uv.y *= -1;
    
    // x, -x, y, -y, z, -z
    switch (faceIndex)
    {
    case 0:
        return float3(1, uv.y, -uv.x);
    case 1:
        return float3(-1, uv.y, uv.x);
    case 2:
        return float3(uv.x, 1, -uv.y);
    case 3:
        return float3(uv.x, -1, uv.y);
    case 4:
        return float3(uv.x, uv.y, 1);
    case 5:
        return float3(-uv.x, uv.y, 1);
    default:
        return 0;
    }
}

[numthreads(SHADER_MIPMAPGENERATE_BLOCKSIZE, SHADER_MIPMAPGENERATE_BLOCKSIZE, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    // 直接采样上一级的纹理，输出到新一级的纹理中
    if (DTid.x < gMipmapGenResolution.x && DTid.y < gMipmapGenResolution.y)
    {
        float2 uv = (DTid.xy + 0.5f) * gMipmapInverseResolution.xy;
        float3 dir = GetCubeMapDir(uv, DTid.z);
        texture_output[DTid.xyz] = texture_input.SampleLevel(genSampler, dir, 0);
    }
}