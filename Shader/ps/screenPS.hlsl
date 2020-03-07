#include "..\hf\fullScreenHF.hlsli"

float4 main(FullScreenPixelInput input) : SV_TARGET
{
    return screenTexture.SampleLevel(sampler_linear_clamp, input.tex, 0);
}