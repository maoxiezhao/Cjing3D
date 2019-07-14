#include "hf\objectHF.hlsli"
#include "hf\brdf.hlsli"

float4 main(PixelInputType input) : SV_TARGET
{
    float4 color = input.color;

    return color;
}