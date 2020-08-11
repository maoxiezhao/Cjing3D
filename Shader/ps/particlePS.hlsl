#include "..\hf\particleHF.hlsli"
#include "..\hf\lightingHF.hlsli"

float4 main(VertexToPixel input) : SV_TARGET
{
    float4 color = texture_0.Sample(sampler_linear_clamp, input.tex.xy);
    clip(color.a - 1.0f / 255.0f);
	
    float4 inputColor = ConvertColorUintToFloat4(input.color);
    color.rgb *= inputColor.rgb;
    color.a = saturate(color.a * inputColor.a);
    
    return color;
}