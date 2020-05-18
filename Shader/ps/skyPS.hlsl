#include "..\hf\global.hlsli"

struct SkyPSIn
{
    float4 pos : SV_Position;
    float2 tex : TEXCOORD0;
};

float4 main(SkyPSIn input) : SV_TARGET
{
    float4 worldPos = mul(gCameraInvVP, float4(input.tex.xy, 0.0f, 1.0f));
    worldPos.xyz /= worldPos.w;
    
    float3 vDir = normalize(worldPos.xyz - gCameraPos);
    return float4(DeGammaCorrectSky(texture_global_env_map.SampleLevel(sampler_linear_clamp, vDir, 0).rgb), 1.0f);
}