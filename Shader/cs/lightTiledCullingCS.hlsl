#include "..\hf\tiledCullingHF.hlsli"

//#define TILED_CULLING_DEBUG

STRUCTUREDBUFFER(TiledFrustums, TiledFrustum, SBSLOT_TILED_FRUSTUMS);
RWSTRUCTUREDBUFFER(RWTiledLights, uint, 0);

#ifdef TILED_CULLING_DEBUG
RWTEXTURE2D(RWDebugTexture, unorm float4, 1);
#endif

groupshared uint uMinDepth;
groupshared uint uMaxDepth;
groupshared uint uTiledLights[SHADER_LIGHT_TILE_BUCKET_COUNT];
groupshared uint uDebugTiledLightCount;
groupshared AABB uTileAABB;
groupshared uint uDepthMask;

// 2.5D Culling： 因为深度不是连续的，当光源在深度中相交，且部分像素中间未中间深度，该Culling可以进一步剔除
// 将光源Sphere构建到DepthMask:
// 计算sphere的最大最小深度，映射到DepthMask上0000111100000
inline uint ConstructDepthMask(Sphere sphere, float viewMinDepth, float invDepthRange)
{
    float minDepth = sphere.center.z - sphere.radius;
    float maxDepth = sphere.center.z + sphere.radius;
    uint depthMaskStart = max(0.0f, min(31.0f, (minDepth - viewMinDepth) * invDepthRange));
    uint depthMaskEnd = max(0.0f, min(31.0f, (maxDepth - viewMinDepth) * invDepthRange));
    
    uint depthMask = 0xffffffff;
    depthMask >>= (31 - (depthMaskEnd - depthMaskStart));
    depthMask <<= depthMaskStart;
    
    return depthMask;
}

inline void AddTiledLight(uint index)
{
    // index以32位二进制位存储
    const uint realIndex = index / 32;
    const uint realBit = index % 32;
    InterlockedOr(uTiledLights[realIndex], 1 << realBit);

#ifdef TILED_CULLING_DEBUG
    InterlockedAdd(uDebugTiledLightCount, 1);
#endif    
}

[numthreads(LIGHT_CULLING_THREAD_SIZE, LIGHT_CULLING_THREAD_SIZE, 1)]
void main(uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint groupIndex : SV_GroupIndex)
{
    // 每个group对应于一个tile，可以得到tile的frustum，同时为了得到每个tile的最大和最小深度
    // SV_GroupThreadID可以最终计算出对应UV，从depthBuffer中得到深度
 
    const uint tileIndex = Flatten(Gid.xy, gCSNumThreadGroups);
    const uint lightBucketAddress = tileIndex * SHADER_LIGHT_TILE_BUCKET_COUNT;
    TiledFrustum tiledFrustum = TiledFrustums[tileIndex];
    
    // 1. 初始化
    uint bucketIndex = groupIndex;
    uint i = 0;
    uint step = LIGHT_CULLING_THREAD_SIZE * LIGHT_CULLING_THREAD_SIZE;
    for (i = bucketIndex; i < SHADER_LIGHT_TILE_BUCKET_COUNT; i += step)
    {
        uTiledLights[i] = 0;
    }
    if (groupIndex == 0)
    {
        uMaxDepth = 0;
        uMinDepth = 0xffffffff;
        uDepthMask = 0;
#ifdef TILED_CULLING_DEBUG
        uDebugTiledLightCount = 0;
#endif
    }
    
    // 2.计算tile最大和最小深度
    float currentMinDepth =  10000000;
    float currentMaxDepth = -10000000;
    float currentDepth[LIGHT_CULLING_THREAD_SIZE * LIGHT_CULLING_THREAD_SIZE];
    
    // 通过粒度控制减少所需的threadCount
    uint granularity = 0;
    uint granularityStep = LIGHT_CULLING_GRANULARITY * LIGHT_CULLING_GRANULARITY;
    [unroll]
    for (granularity = 0; granularity < granularityStep; granularity++)
    {
        uint2 pixel = DTid.xy * uint2(LIGHT_CULLING_GRANULARITY, LIGHT_CULLING_GRANULARITY) + UnFlatten(granularity, LIGHT_CULLING_GRANULARITY);
        pixel = min(pixel, GetScreenSize() - 1.0f);
        currentDepth[granularity] = texture_depth[pixel];
        currentMaxDepth = max(currentMaxDepth, currentDepth[granularity]);
        currentMinDepth = min(currentMinDepth, currentDepth[granularity]);
    }
    
    GroupMemoryBarrierWithGroupSync();
    
    InterlockedMin(uMinDepth, asuint(currentMinDepth));
    InterlockedMax(uMaxDepth, asuint(currentMaxDepth));
    
    GroupMemoryBarrierWithGroupSync();
    
    // reversed zbuffer
    float zMinDepth = asfloat(uMaxDepth);
    float zMaxDepth = asfloat(uMinDepth);
    
    float viewMinDepth = ScreenToView(float4(0.0f, 0.0f, zMinDepth, 1.0f)).z;
    float viewMaxDepth = ScreenToView(float4(0.0f, 0.0f, zMaxDepth, 1.0f)).z;
    float viewNearClip = ScreenToView(float4(0.0f, 0.0f, 1.0f, 1.0f)).z;
    
    // 2.5D Culling
    // 将minDepth到maxDepth之间划分为32部分[0-31]，当存在当前深度时，则将对应为设置为1 
    const float invDepthRange = 31.0f / (viewMaxDepth - viewMinDepth);
    uint currentDepthMask = 0;
    [unroll]
    for (granularity = 0; granularity < granularityStep; granularity++)
    {
        float viewDepth = ScreenToView(float4(0.0f, 0.0f, currentDepth[granularity], 1.0f)).z;
        uint bitPlace = max(0.0f, min(31.0f, (viewDepth - viewMinDepth) * invDepthRange));
        currentDepthMask |= 1 << bitPlace;
    }
    InterlockedOr(uDepthMask, currentDepthMask);
        
    // 大量非对称frustum导致culling精度过低，这里对每个tile构建更紧密的包围盒
    if(groupIndex == 0)
    {
        float3 viewCorners[8];
        viewCorners[0] = ScreenToView(float4(float2(Gid.x, Gid.y) * LIGHT_CULLING_TILED_BLOCK_SIZE, zMinDepth, 1.0f)).xyz;
        viewCorners[1] = ScreenToView(float4(float2(Gid.x + 1, Gid.y) * LIGHT_CULLING_TILED_BLOCK_SIZE, zMinDepth, 1.0f)).xyz;
        viewCorners[2] = ScreenToView(float4(float2(Gid.x, Gid.y + 1) * LIGHT_CULLING_TILED_BLOCK_SIZE, zMinDepth, 1.0f)).xyz;
        viewCorners[3] = ScreenToView(float4(float2(Gid.x + 1, Gid.y + 1) * LIGHT_CULLING_TILED_BLOCK_SIZE, zMinDepth, 1.0f)).xyz;
        
        viewCorners[4] = ScreenToView(float4(float2(Gid.x, Gid.y) * LIGHT_CULLING_TILED_BLOCK_SIZE, zMaxDepth, 1.0f)).xyz;
        viewCorners[5] = ScreenToView(float4(float2(Gid.x + 1, Gid.y) * LIGHT_CULLING_TILED_BLOCK_SIZE, zMaxDepth, 1.0f)).xyz;
        viewCorners[6] = ScreenToView(float4(float2(Gid.x, Gid.y + 1) * LIGHT_CULLING_TILED_BLOCK_SIZE, zMaxDepth, 1.0f)).xyz;
        viewCorners[7] = ScreenToView(float4(float2(Gid.x + 1, Gid.y + 1) * LIGHT_CULLING_TILED_BLOCK_SIZE, zMaxDepth, 1.0f)).xyz;
        
        float3 minV = 10000000;
        float3 maxV = -10000000;
        [unroll]
        for (int i = 0; i < 8; i++)
        {
            minV = min(minV, viewCorners[i]);
            maxV = max(maxV, viewCorners[i]);
        }
        CreateAABBFromMinMax(uTileAABB, minV, maxV);
    }
    
    GroupMemoryBarrierWithGroupSync();
    
    // 3.遍历所有light，判断是否和frustum相交
    // 每个groupThread只处理（lightCount / step)个light
    for (i = groupIndex; i < gCSNumLights; i += step)
    {
        ShaderLight light = LightArray[i];
		
        switch (light.GetLightType())
        {
            case SHADER_LIGHT_TYPE_DIRECTIONAL:
                AddTiledLight(i);
                break;
            case SHADER_LIGHT_TYPE_POINT:
            {
                Sphere sphere = { light.viewPosition, light.range };
                if (CheckSphereInsideFrustum(sphere, tiledFrustum, viewNearClip, viewMaxDepth))
                {
                    if (CheckSphereIntersectsAABB(sphere, uTileAABB))
                    {
                        // 2.5D Culling
                        if (uDepthMask & ConstructDepthMask(sphere, viewMinDepth, invDepthRange)) {
                            AddTiledLight(i);
                        }
                    }
                }
            }
            break;
        }
    }
    
    GroupMemoryBarrierWithGroupSync();
    
    // 4. 写入rwTiledLights
    for (i = bucketIndex; i < SHADER_LIGHT_TILE_BUCKET_COUNT; i += step)
    {
        RWTiledLights[lightBucketAddress + i] = uTiledLights[i];
    }
    
    // debug
#ifdef TILED_CULLING_DEBUG
    for (granularity = 0; granularity < granularityStep; granularity++)
    {
        uint2 debugPixel = DTid.xy * uint2(LIGHT_CULLING_GRANULARITY, LIGHT_CULLING_GRANULARITY) + UnFlatten(granularity, LIGHT_CULLING_GRANULARITY);
        
        const float3 mapTex[] =
        {
            float3(0, 0, 0),
		    float3(0, 0, 1),
		    float3(0, 1, 1),
		    float3(0, 1, 0),
		    float3(1, 1, 0),
		    float3(1, 0, 0),
        };
        const uint mapTexLen = 5;
        const uint maxHeat = 50;
        float p = saturate((float) uDebugTiledLightCount / maxHeat);
        float l = p * mapTexLen;
        float3 a = mapTex[floor(l)];
        float3 b = mapTex[ceil(l)];
        float4 heatmap = float4(lerp(a, b, l - floor(l)), p);
        RWDebugTexture[debugPixel] = heatmap;
    }
#endif    
}