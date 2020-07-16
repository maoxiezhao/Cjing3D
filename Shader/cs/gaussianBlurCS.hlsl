#include "..\hf\global.hlsli"

#ifndef BLUR_FORMAT
#define BLUR_FORMAT float
#endif // BLUR_FORMAT

TYPE_TEXTURE2D(texture_input, BLUR_FORMAT, TEXTURE_SLOT_UNIQUE_0);
RWTEXTURE2D(texture_output, BLUR_FORMAT, 0);


#define BLUR_WIDE

#ifdef BLUR_WIDE
static const uint gaussKernel = 33;
static const float gaussianWeights[gaussKernel] = { 
    0.004013,0.005554,0.007527,0.00999,0.012984,0.016524,0.020594,0.025133,0.030036,0.035151,0.040283,
	0.045207,0.049681,0.053463,0.056341,0.058141,0.058754,0.058141,0.056341,0.053463,0.049681,0.045207,
	0.040283,0.035151,0.030036,0.025133,0.020594,0.016524,0.012984,0.00999,0.007527,0.005554,0.004013
};
static const int gaussianOffsets[gaussKernel] = { 
    -16,-15,-14,-13,-12,-11,-10,-9,-8,-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
};
#else
static const uint gaussKernel = 9;
static const float gaussianWeights[gaussKernel] = { 0.004112, 0.026563, 0.100519, 0.223215, 0.29118, 0.223215, 0.100519, 0.026563, 0.004112 };
static const int gaussianOffsets[gaussKernel] = { -4, -3, -2, -1, 0, 1, 2, 3, 4 };
#endif

static const uint BorderSize = gaussKernel / 2;
static const uint TileSize = BorderSize * 2 + SHADER_GAUSSIAN_BLUR_BLOCKSIZE;

groupshared BLUR_FORMAT colorCache[TileSize];

#ifdef BILATERAL_BLUR
groupshared float depthCache[TileSize];
#endif

[numthreads(SHADER_GAUSSIAN_BLUR_BLOCKSIZE, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID, uint groupIndex : SV_GroupIndex, uint3 Gid : SV_GroupID)
{
    // GaussianBlur 
    // 每次只对一个方向做高斯模糊，在每个group中先将所有颜色取出cache(减少sample texture次数）
    // 再依次对每个像素和高斯核做卷积
    // 当考虑BilateralBlur滤波时，在gaussianBlur的基础上再根据灰度（深度）差值计算权重
    // 在这里我们只处理深度信息(用于SSAO),且简化为仅在两点depth差值大于depthThreshold时滤波
    float2 direction = float2(
        gPPParam2,
        gPPParam3
    );
    uint2 startPos = Gid.xy;
    [flatten]
    if (direction.x == 0)
    {
        startPos.y *= SHADER_GAUSSIAN_BLUR_BLOCKSIZE;
    }
    else
    {
        startPos.x *= SHADER_GAUSSIAN_BLUR_BLOCKSIZE;
    }
        
    for (uint index = groupIndex; index < TileSize; index += SHADER_GAUSSIAN_BLUR_BLOCKSIZE)
    {
        const float2 uv = (startPos + direction * (index - BorderSize) + 0.5f) * gPPInverseResolution;
        colorCache[index] = texture_input.SampleLevel(sampler_linear_clamp, uv, 0.0f);
        
#ifdef BILATERAL_BLUR
        depthCache[index] = texture_linear_depth.SampleLevel(sampler_point_clamp, uv, 0.0f);
#endif
    }
    GroupMemoryBarrierWithGroupSync();

    const uint2 currentPos = startPos + groupIndex * direction;
    if (currentPos.x >= gPPResolution.x || currentPos.y >= gPPResolution.y) {
        return;
    }
    
    const uint centerIndex = groupIndex + BorderSize;
    
#ifdef BILATERAL_BLUR
    const float depthThreshold = gPPParam1;
    const float centerDepth = depthCache[centerIndex];
    const BLUR_FORMAT centerColor = colorCache[centerIndex];
#endif  
    
    BLUR_FORMAT color = 0.0f;
    for (uint i = 0; i < gaussKernel; i++)
    {
        const uint index = centerIndex + gaussianOffsets[i];
        BLUR_FORMAT curColor = colorCache[index];
        
#ifdef BILATERAL_BLUR
        // 在这里我们只处理深度信息(用于SSAO),且简化为仅在两点depth差值大于depthThreshold时滤波
        // 表现为 lerp(curColor, centerDepth, weight)
        float curDepth = depthCache[index];
        float weight = saturate(abs(curDepth - centerDepth) * gCameraFarZ * depthThreshold);
        color += lerp(curColor, centerColor, weight) * gaussianWeights[i];
#else
        color += curColor * gaussianWeights[i];  
#endif
    }
    
    texture_output[currentPos] = color;
}