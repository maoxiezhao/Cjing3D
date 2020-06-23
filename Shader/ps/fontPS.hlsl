#include "..\hf\imageHF.hlsli"

struct FontPixelInput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};


float4 main(FontPixelInput input) : SV_TARGET
{
    return texture_base.SampleLevel(sampler_linear_clamp, input.uv, 0).rrrr * gFontColor;
}