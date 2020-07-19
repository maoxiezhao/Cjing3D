#include "..\hf\global.hlsli"

//#define DEBUG_NORMAL_TEXTURE

RWTEXTURE2D(texture_output, unorm float, 0);

#ifdef DEBUG_NORMAL_TEXTURE
RWTEXTURE2D(texture_normal_output, float4, 1);
#endif


// 因为每个像素需要取neighbour4个采样点，所以需要1像素的border
static const uint TILE_BORDER = 1;
static const uint TILE_SIZE = TILE_BORDER * 2 + SHADER_POSTPROCESS_BLOCKSIZE;
static const uint TILE_BLOCK_SIZE = TILE_SIZE * TILE_SIZE;

groupshared float2 tileViewXY[TILE_BLOCK_SIZE];
groupshared float tileViewZ[TILE_BLOCK_SIZE];

[numthreads(SHADER_POSTPROCESS_BLOCKSIZE, SHADER_POSTPROCESS_BLOCKSIZE, 1)]
void main(uint3 DTid : SV_DispatchThreadID, uint3 Gid : SV_GroupID, uint3 GTid : SV_GroupThreadID, uint groupIndex : SV_GroupIndex)
{
    // SSAO:
    // 对于每个像素，在切线空间球内构建随机采样点，再将采样点转换到相机空间
    // 得到该采样点的深度，根据采样点屏幕空间坐标可以从depthBuffer中得到对应位置的
    // 深度信息，当得到深度信息大于采样点的深度时，增加遮蔽值
    const uint aoRange = (uint)gPPParam1;
    const uint aoSampleCount = (uint) gPPParam2;
    
    // 从depthBuffer构建法线
    // https://wickedengine.net/2019/09/22/improved-normal-reconstruction-from-depth/
    // 这需要从DepthBuffer中选取出3个tabs来构建法线，但是任意选取3点可能会导致artifactss
    // 所以对周围4个点采样，选择深度差值最小的2个点来构建法线
    
    // 1. 重建每个像素的view pos
    const int2 tileLeftUpPos = Gid.xy * SHADER_POSTPROCESS_BLOCKSIZE - TILE_BORDER;
    for (uint index = groupIndex; index < TILE_BLOCK_SIZE; index += SHADER_POSTPROCESS_BLOCKSIZE * SHADER_POSTPROCESS_BLOCKSIZE)
    {
        const float2 uv = (tileLeftUpPos + UnFlatten(index, TILE_SIZE) + 0.5f) * gPPInverseResolution;
        const float depth = texture_depth.SampleLevel(sampler_linear_clamp, uv, 0);
        const float3 pos = ScreenToCamera(uv, depth);

        tileViewXY[index] = pos.xy;
        tileViewZ[index] = pos.z;
    }
    GroupMemoryBarrierWithGroupSync();
    
    // 2. select best point
    const uint crossIdx[5] =
    {
        Flatten(TILE_BORDER + GTid.xy, TILE_SIZE),               // 0: center
		Flatten(TILE_BORDER + GTid.xy + int2(1, 0), TILE_SIZE),  // 1: right
		Flatten(TILE_BORDER + GTid.xy + int2(-1, 0), TILE_SIZE), // 2: left
		Flatten(TILE_BORDER + GTid.xy + int2(0, 1), TILE_SIZE),  // 3: down
		Flatten(TILE_BORDER + GTid.xy + int2(0, -1), TILE_SIZE), // 4: up
    };
    const float zCenter = tileViewZ[crossIdx[0]];
    [branch]
    if (zCenter >= gCameraFarZ){
        texture_output[DTid.xy] = 1.0f;
        
#ifdef DEBUG_NORMAL_TEXTURE
        texture_normal_output[DTid.xy] = 0.0f;
#endif  
        return;
    }
    
    float zRight = tileViewZ[crossIdx[1]];
    float zLeft = tileViewZ[crossIdx[2]];
    float zDown = tileViewZ[crossIdx[3]];
    float zUp = tileViewZ[crossIdx[4]];
    
    uint bestHorizontalIndex = abs(zRight - zCenter) < abs(zLeft - zCenter) ? 1 : 2;
    uint bestVerticalIndex   = abs(zDown - zCenter) < abs(zUp - zCenter) ? 3 : 4;
    
    // 注意cross后的法线方向
    float3 p1 = 0.0f, p2 = 0.0f;
    if (bestHorizontalIndex == 1 && bestVerticalIndex == 3)
    {
        p1 = float3(tileViewXY[crossIdx[3]], zDown);
        p2 = float3(tileViewXY[crossIdx[1]], zRight);

    }
    else if (bestHorizontalIndex == 1 && bestVerticalIndex == 4)
    {
        p1 = float3(tileViewXY[crossIdx[1]], zRight);
        p2 = float3(tileViewXY[crossIdx[4]], zUp);
    }
    else if (bestHorizontalIndex == 2 && bestVerticalIndex == 3)
    {
        p1 = float3(tileViewXY[crossIdx[2]], zLeft);
        p2 = float3(tileViewXY[crossIdx[3]], zDown);
    }
    else if (bestHorizontalIndex == 2 && bestVerticalIndex == 4)
    {
        p1 = float3(tileViewXY[crossIdx[4]], zUp);
        p2 = float3(tileViewXY[crossIdx[2]], zLeft);
    }
    
    // 3. compute normal
    float3 currentPos = float3(tileViewXY[crossIdx[0]], zCenter);
    float3 normal = normalize(cross(p2 - currentPos, p1 - currentPos));
    float2 pixel = (DTid.xy + 0.5f) * gPPInverseResolution;
    
    
#ifdef DEBUG_NORMAL_TEXTURE
    texture_normal_output[DTid.xy] = float4(normal.xyz, 1.0f);
#endif
    
    float seed = 1.0f;
    float3 randomVec = float3(Rand(seed, pixel), Rand(seed, pixel), Rand(seed, pixel)) * 2.0f - 1.0f;  // => []-1, 1]
    float3 tagent = normalize(randomVec - normal * dot(normal, randomVec)); // Gramm-Schmidt
    float3 bitTagent = cross(normal, tagent);
    float3x3 TBN = float3x3(tagent, bitTagent, normal);
    
    float totalAO = 0.0f;
    for (uint i = 0; i < aoSampleCount; i++)
    {
        // 从半球内均匀获取采样点，并转换到相机空间
        float2 hamPoint = Hammersley(i, aoSampleCount);
        float3 hemSpherePoint = HemisphereSampleUniform(hamPoint.x, hamPoint.y);
        float3 viewPoint = mul(hemSpherePoint, TBN);
        // 扰动一下最终采样点位置
        float range = aoRange * lerp(0.2f, 1.0f, Rand(seed, pixel));
        float3 samplePos = currentPos + viewPoint * range;

        // xy转换到屏幕空间
        float4 clipPos = mul(gCameraProj, float4(samplePos, 1.0f));
        clipPos.xyz /= clipPos.w;     
        // x[-1, 1] y[-1, 1] -> [0, 1]，且y需要当做v作为纹理坐标，需要反转
        clipPos.xy = clipPos.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f); 
        
        [branch]
        if (IsSaturated(clipPos.xy))
        {
            // 这里都是View下的z坐标
            const float sampleDepth = texture_linear_depth.SampleLevel(sampler_point_clamp, clipPos.xy, 0.0f).r * gCameraFarZ;
            const float sampleRealDepth = clipPos.w;
            
            // check range;只处理一定深度范围内点
            float rangeCheck = 1 - saturate(abs(sampleRealDepth - sampleDepth) * 0.2f);
            totalAO += (sampleDepth < sampleRealDepth) * rangeCheck;
        }
    }
    
    totalAO /= aoSampleCount;
    texture_output[DTid.xy] = saturate(1.0f - totalAO);
}