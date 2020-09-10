#include "..\hf\global.hlsli"

#ifndef BLUR_FORMAT
#define BLUR_FORMAT float
#endif // BLUR_FORMAT

TEXTURE2D(texture_input, BLUR_FORMAT, TEXTURE_SLOT_UNIQUE_0);
RWTEXTURE2D(texture_output, BLUR_FORMAT, 0);

[numthreads(SHADER_POSTPROCESS_BLOCKSIZE, SHADER_POSTPROCESS_BLOCKSIZE, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    // bilateralUpsample
    // 比较highres和对应lowres的4个点深度差值
    // 1： 如果深度差值过大（可能存在边缘），则选取深度最接近的点
    // 2： 如果深度差值小于depthThreshold，则对4个点像素做双线性差值
    
    const float2 lowTexelUV = float2(gPPParam1, gPPParam2);
    const float depthThreshold = gPPParam3;
    const float2 uv = (DTid.xy + 0.5f) * gPPInverseResolution;
    
    const float2 uv0 = uv  - lowTexelUV * 0.5f;                  // lt
    const float2 uv1 = uv0 + float2(lowTexelUV.x, 0.0f); // rt
    const float2 uv2 = uv0 + float2(0.0f, lowTexelUV.y); // lb
    const float2 uv3 = uv0 + lowTexelUV; // rb
 
    const float4 depthHighres = texture_linear_depth.SampleLevel(sampler_point_clamp, uv, 0.0f).xxxx;
    const float4 depthLowres = float4(
        texture_linear_depth.SampleLevel(sampler_point_clamp, uv0, 0.0f),
        texture_linear_depth.SampleLevel(sampler_point_clamp, uv1, 0.0f),
        texture_linear_depth.SampleLevel(sampler_point_clamp, uv2, 0.0f),
        texture_linear_depth.SampleLevel(sampler_point_clamp, uv3, 0.0f)
    );
    const float4 depthDiff = (depthHighres - depthLowres) * gCameraFarZ;
    
    BLUR_FORMAT color = 0.0f;
    [branch]
    if (dot(depthDiff, float4(1.0f, 1.0f, 1.0f, 1.0f)) <= depthThreshold)
    {
        color = texture_input.SampleLevel(sampler_linear_clamp, uv, 0.0f);
    }
    else
    {
        float minimumDepthDiff = depthDiff[0];
        float2 closetUV = uv0;
        
        if (depthDiff[1] < minimumDepthDiff)
        {
            minimumDepthDiff = depthDiff[1];
            closetUV = uv1;
        }
        if (depthDiff[2] < minimumDepthDiff)
        {
            minimumDepthDiff = depthDiff[2];
            closetUV = uv2;
        }
        if (depthDiff[3] < minimumDepthDiff)
        {
            minimumDepthDiff = depthDiff[3];
            closetUV = uv3;
        }
        
        color = texture_input.SampleLevel(sampler_point_clamp, closetUV, 0.0f);
    }
    
    texture_output[DTid.xy] = color;
}