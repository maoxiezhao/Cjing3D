#include "..\hf\imageHF.hlsli"

float4 main(SpritePixelInput input) : SV_TARGET
{
    return texture_base.SampleLevel(sampler_linear_clamp, input.uv, 0.0f) * input.color;
}