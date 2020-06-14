#include "..\hf\tiledCullingHF.hlsli"

#define TILED_CULLING_DEBUG

STRUCTUREDBUFFER(TiledFrustums, TiledFrustum, SBSLOT_TILED_FRUSTUMS);
RWSTRUCTUREDBUFFER(RWTiledLights, uint, 0);

#ifdef TILED_CULLING_DEBUG
RWTEXTURE2D(RWDebugTexture, unorm float4, 1);
#endif

groupshared uint uMinDepth;
groupshared uint uMaxDepth;
groupshared uint uTiledLights[SHADER_LIGHT_TILE_BUCKET_COUNT];
groupshared uint uDebugTiledLightCount;

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
    TiledFrustum tiledFrustum = TiledFrustums[tileIndex];   
    const uint lightBucketAddress = tileIndex * SHADER_LIGHT_TILE_BUCKET_COUNT;
 
    // 1. 初始化
    uint bucketIndex = groupIndex;
    uint i = 0;
    uint step = LIGHT_CULLING_THREAD_SIZE * LIGHT_CULLING_THREAD_SIZE;
    for (i = bucketIndex; i < SHADER_LIGHT_COUNT; i += step)
    {
        uTiledLights[i] = 0;
    }
    if (groupIndex == 0)
    {
        uMaxDepth = 0;
        uMinDepth = 0xffffffff;
        
#ifdef TILED_CULLING_DEBUG
        uDebugTiledLightCount = 0;
#endif
    }
    
    // 2.计算tile最大和最小深度
    float currentMinDepth =  10000000;
    float currentMaxDepth = -10000000;
    float currentDepth = 0.0f;
    
    uint2 pixel = DTid.xy;
    pixel = min(pixel, GetScreenSize() - 1.0f);
    currentDepth = texture_depth[pixel];
    currentMaxDepth = max(currentMaxDepth, currentDepth);
    currentMinDepth = min(currentMinDepth, currentDepth);
    
    GroupMemoryBarrierWithGroupSync();
    
    InterlockedMin(uMinDepth, asuint(currentMinDepth));
    InterlockedMax(uMaxDepth, asuint(currentMaxDepth));
    
    GroupMemoryBarrierWithGroupSync();
    
    // reversed zbuffer
    float zMinDepth = uMaxDepth;
    float zMaxDepth = uMinDepth;
    
    float viewMinDepth = ScreenToView(float4(0.0f, 0.0f, zMinDepth, 1.0f)).z;
    float viewMaxDepth = ScreenToView(float4(0.0f, 0.0f, zMaxDepth, 1.0f)).z;
    float viewNearClip = ScreenToView(float4(0.0f, 0.0f, 1.0f, 1.0f)).z;
    
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
                    AddTiledLight(i);
                }
            }
            break;
        }
    }
    
    GroupMemoryBarrierWithGroupSync();
    
    // 4. 写入rwTiledLights
    for (i = bucketIndex; i < SHADER_LIGHT_COUNT; i += step)
    {
        RWTiledLights[lightBucketAddress + i] = uTiledLights[i];
    }
    
    // debug
#ifdef TILED_CULLING_DEBUG
    uint2 debugPixel = DTid.xy;
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
    const uint maxHeat = 5;
    float p = saturate((float) uDebugTiledLightCount / maxHeat);
    float l = p * mapTexLen;
    float3 a = mapTex[floor(l)];
    float3 b = mapTex[ceil(l)];
    float4 heatmap = float4(lerp(a, b, l - floor(l)), p);
    RWDebugTexture[debugPixel] = heatmap;
    
#endif    
}