#include "..\hf\global.hlsli"

#ifndef TARGET_FORMAT
#define TARGET_FORMAT float4
#endif

TYPE_TEXTURE2D(texture_input, float4, TEXTURE_SLOT_UNIQUE_0);
RWTEXTURE2D(texture_output, TARGET_FORMAT, 0);
SAMPLERSTATE(genSampler, SAMPLER_SLOT_0);

[numthreads(SHADER_MIPMAPGENERATE_BLOCKSIZE, SHADER_MIPMAPGENERATE_BLOCKSIZE, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    // 直接采样上一级的纹理，输出到新一级的纹理中
    if (DTid.x < gMipmapGenResolution.x && DTid.y < gMipmapGenResolution.y)
    {
        float2 uv = (DTid.xy + 0.5f) * gMipmapInverseResolution.xy;
        texture_output[DTid.xy] = texture_input.SampleLevel(genSampler, uv, 0);
    }
}