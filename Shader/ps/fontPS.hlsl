#include "..\hf\imageHF.hlsli"

SAMPLERSTATE(sampler_font, SAMPLER_SLOT_0);
TEXTURE2D(texture_font, float, TEXTURE_SLOT_FONT);

struct FontPixelInput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float4 main(FontPixelInput input) : SV_TARGET
{
    return texture_font.SampleLevel(sampler_font, input.uv, 0).rrrr * gFontColor;
}