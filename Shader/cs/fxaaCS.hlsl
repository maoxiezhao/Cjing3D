#include "..\hf\global.hlsli"


#define FXAA_PC 1
#define FXAA_HLSL_5 1
#define FXAA_QUALITY__PRESET 25
#define FXAA_GREEN_AS_LUMA 1
#include "..\hf\nvidiaFxaa.hlsli"

// Use NVIDIA FXAA.
// Algorithm: http://blog.simonrodriguez.fr/articles/30-07-2016_implementing_fxaa.html

TYPE_TEXTURE2D(texture_input, float4, TEXTURE_SLOT_0);
RWTEXTURE2D(texture_output, unorm float4, 0);

static const float fxaaSubpix = 0.75;
static const float fxaaEdgeThreshold = 0.166;
static const float fxaaEdgeThresholdMin = 0.0833;

[numthreads(SHADER_POSTPROCESS_BLOCKSIZE, SHADER_POSTPROCESS_BLOCKSIZE, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    float2 uv = (DTid.xy + 0.5f) * gPPInverseResolution;
    FxaaTex fxaaTex = { sampler_linear_clamp, texture_input };
   
    texture_output[DTid.xy] = FxaaPixelShader(uv, 0, fxaaTex, fxaaTex, fxaaTex, gPPInverseResolution,
        0, 0, 0, fxaaSubpix, fxaaEdgeThreshold, fxaaEdgeThresholdMin, 0, 0, 0, 0);
}