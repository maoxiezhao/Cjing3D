#include "..\hf\imageHF.hlsli"

struct FontPixelInput
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

FontPixelInput main(float2 pos : POSITION, float2 tex : TEXCOORD0)
{
    FontPixelInput Out;
    Out.pos = mul(gFontTransform, float4(pos.xy, 0.0f, 1.0f));
    Out.uv = tex;
    return Out;
}