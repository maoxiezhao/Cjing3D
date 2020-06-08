#include "..\hf\tiledCullingHF.hlsli"

RWSTRUCTUREDBUFFER(TiledFrustums, TiledFrustum, 0);

[numthreads(LIGHT_CULLING_TILED_BLOCK_SIZE, LIGHT_CULLING_TILED_BLOCK_SIZE, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    // ���������ϵ�£���tile����frustum������ֻ��Ҫ��tile���ĸ��ǹ���4���棬Զ���ü�
    // ����Ϊ��ֱ��z�ᣬֻ��Ҫ��LightCulling�׶�ֱ�ӻ�ȡtile��zNear��zFar����
  
    float3 cameraPos = float3(0.0f, 0.0f, 0.0f);
    
    // ��ȡtile��4��������  
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
    
    // ����4����ķ���ax + by + cz + d
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