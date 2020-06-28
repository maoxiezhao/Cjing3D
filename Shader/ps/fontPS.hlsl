#include "..\hf\imageHF.hlsli"

SAMPLERSTATE(sampler_font, SAMPLER_SLOT_0);

struct FontPixelInput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float4 main(FontPixelInput input) : SV_TARGET
{
    return texture_base.SampleLevel(sampler_font, input.uv, 0).rrrr * gFontColor;
}