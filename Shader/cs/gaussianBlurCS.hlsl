#include "..\hf\global.hlsli"

#ifndef BLUR_FORMAT
#define BLUR_FORMAT float
#endif // BLUR_FORMAT

TYPE_TEXTURE2D(texture_input, BLUR_FORMAT, TEXTURE_SLOT_UNIQUE_0);
RWTEXTURE2D(texture_output, BLUR_FORMAT, 0);

static const uint gaussKernel = 9;
static const float gaussianWeights[gaussKernel] = { 0.004112, 0.026563, 0.100519, 0.223215, 0.29118, 0.223215, 0.100519, 0.026563, 0.004112};
static const int gaussianOffsets[gaussKernel] = { -4, -3, -2, -1, 0, 1, 2, 3, 4 };

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
    // ÿ��ֻ��һ����������˹ģ������ÿ��group���Ƚ�������ɫȡ��cache(����sample texture������
    // �����ζ�ÿ�����غ͸�˹��������
    // ������BilateralBlur�˲�ʱ����gaussianBlur�Ļ������ٸ��ݻҶȣ���ȣ���ֵ����Ȩ��
    // ����������ֻ���������Ϣ(����SSAO),�Ҽ�Ϊ��������depth��ֵ����depthThresholdʱ�˲�
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
        // ����������ֻ���������Ϣ(����SSAO),�Ҽ�Ϊ��������depth��ֵ����depthThresholdʱ�˲�
        // ����Ϊ lerp(curColor, centerDepth, weight)
        float curDepth = depthCache[index];
        float weight = saturate(abs(curDepth - centerDepth) * gCameraFarZ * depthThreshold);
        color += lerp(curColor, centerColor, weight) * gaussianWeights[i];
#else
        color += curColor * gaussianWeights[i];  
#endif
    }
    
    texture_output[currentPos] = color;
}