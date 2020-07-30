#include "..\hf\global.hlsli"

TEXTURE2D(texture_input,     float4, TEXTURE_SLOT_UNIQUE_0);
TEXTURE2D(texture_luminance, float, TEXTURE_SLOT_UNIQUE_1);

RWTEXTURE2D(texture_output, unorm float4, 0);

float3 ReinhardTonemap(float3 x)
{
    return x / (x + 1); 
}

//TODO: use better tonemapping

[numthreads(SHADER_POSTPROCESS_BLOCKSIZE, SHADER_POSTPROCESS_BLOCKSIZE, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    float2 uv = (DTid.xy + 0.5f) * gPPInverseResolution;
    float exposure = gPPParam1.x;
    
    float4 hdr = texture_input.SampleLevel(sampler_linear_clamp, uv, 0);
    hdr.rgb *= exposure;
    
    // ��ʱֱ��ʹ��Gray��ʾƽ�����ȣ��Ժ���texture_luminance�л�ȡ
    // texture_luminance��ӿͻ��˴��룬������eye adaptation
    float averageLuminace = 1.0f;
    float luminance = RGBToLuminance(hdr.rgb);
    
    hdr.rgb *= (luminance / averageLuminace);
    
    float4 ldr = saturate(float4(ReinhardTonemap(hdr.rgb), hdr.a));
    ldr.rgb = GammaCorrect(ldr.rgb);
    
    texture_output[DTid.xy] = ldr;
}