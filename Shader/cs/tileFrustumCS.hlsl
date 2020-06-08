#include "..\hf\tiledCullingHF.hlsli"

RWSTRUCTUREDBUFFER(TiledFrustums, TiledFrustum, 0);

[numthreads(LIGHT_CULLING_TILED_BLOCK_SIZE, LIGHT_CULLING_TILED_BLOCK_SIZE, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    // 在相机坐标系下，以tile构建frustum，这里只需要以tile的四个角构建4个面，远近裁剪
    // 面因为垂直于z轴，只需要在LightCulling阶段直接获取tile的zNear和zFar即可
  
    float3 cameraPos = float3(0.0f, 0.0f, 0.0f);
    
    // 获取tile的4个角坐标  
    float4 screenPos[4];
    screenPos[0] = float4(float2(DTid.x, DTid.y) *         LIGHT_CULLING_TILED_BLOCK_SIZE, 1.0f, 1.0f);
    screenPos[1] = float4(float2(DTid.x + 1, DTid.y) *     LIGHT_CULLING_TILED_BLOCK_SIZE, 1.0f, 1.0f);
    screenPos[2] = float4(float2(DTid.x, DTid.y + 1) *     LIGHT_CULLING_TILED_BLOCK_SIZE, 1.0f, 1.0f);
    screenPos[3] = float4(float2(DTid.x + 1, DTid.y + 1) * LIGHT_CULLING_TILED_BLOCK_SIZE, 1.0f, 1.0f);

    float3 viewPos[4];
    for (int i = 0; i < 4; i++)
    {
        viewPos[i] = ScreenToView(screenPos[i]).xyz;
    }
    
    // 构建4个面的方程ax + by + cz + d
    TiledFrustum frustum;
    frustum.planes[0] = ComputePlane(viewPos[0], cameraPos, viewPos[2]);
    frustum.planes[1] = ComputePlane(viewPos[2], cameraPos, viewPos[3]);
    frustum.planes[2] = ComputePlane(viewPos[1], cameraPos, viewPos[3]);
    frustum.planes[3] = ComputePlane(viewPos[0], cameraPos, viewPos[1]);
    
    if (DTid.x < gCSNumThreads.x && DTid.y < gCSNumThreads.y)
    {
        TiledFrustums[Flatten(DTid.xy, gCSNumThreads)] = frustum;
    }
}