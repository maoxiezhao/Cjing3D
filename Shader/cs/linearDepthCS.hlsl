#include "..\hf\global.hlsli"

TYPE_TEXTURE2D(texture_input, float, TEXTURE_SLOT_UNIQUE_0);

RWTEXTURE2D(texture_output_mip0, float, 0);
RWTEXTURE2D(texture_output_mip1, float, 1);
RWTEXTURE2D(texture_output_mip2, float, 2);
RWTEXTURE2D(texture_output_mip3, float, 3);
RWTEXTURE2D(texture_output_mip4, float, 4);
RWTEXTURE2D(texture_output_mip5, float, 5);

groupshared float linearMaxDepth[SHADER_LINEARDEPTH_BLOCKSIZE][SHADER_LINEARDEPTH_BLOCKSIZE];

[numthreads(SHADER_LINEARDEPTH_BLOCKSIZE, SHADER_LINEARDEPTH_BLOCKSIZE, 1)]
void main(uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID)
{
    float2 linearDepthResolution = float2(gPPParam1, gPPParam2);
    
    // SHADER_LINEARDEPTH_BLOCKSIZE大小为depth大小四分之一，即每次计算4个像素
    float depth0 = GetLinearDepth01(texture_input[clamp(DTid.xy * 2 + uint2(0, 0), 0, linearDepthResolution - 1)]);
    float depth1 = GetLinearDepth01(texture_input[clamp(DTid.xy * 2 + uint2(1, 0), 0, linearDepthResolution - 1)]);
    float depth2 = GetLinearDepth01(texture_input[clamp(DTid.xy * 2 + uint2(0, 1), 0, linearDepthResolution - 1)]);
    float depth3 = GetLinearDepth01(texture_input[clamp(DTid.xy * 2 + uint2(1, 1), 0, linearDepthResolution - 1)]);

    texture_output_mip0[DTid.xy * 2 + uint2(0, 0)] = depth0;
    texture_output_mip0[DTid.xy * 2 + uint2(1, 0)] = depth1;
    texture_output_mip0[DTid.xy * 2 + uint2(0, 1)] = depth2;
    texture_output_mip0[DTid.xy * 2 + uint2(1, 1)] = depth3;

    // 直接生成6级mipmap
    float maxDepth = max(depth0, max(depth1, max(depth2, depth3)));
    linearMaxDepth[GTid.x][GTid.y] = maxDepth;
    texture_output_mip1[DTid.xy] = maxDepth;

    GroupMemoryBarrierWithGroupSync();
     
    if (GTid.x % 2 == 0 && GTid.y % 2 == 0)
    {
        maxDepth = max(linearMaxDepth[GTid.x][GTid.y], max(linearMaxDepth[GTid.x + 1][GTid.y], max(linearMaxDepth[GTid.x][GTid.y + 1], linearMaxDepth[GTid.x + 1][GTid.y + 1])));
        linearMaxDepth[GTid.x][GTid.y] = maxDepth;
        texture_output_mip2[DTid.xy / 2] = maxDepth;
    }
    GroupMemoryBarrierWithGroupSync();

    if (GTid.x % 4 == 0 && GTid.y % 4 == 0)
    {
        maxDepth = max(linearMaxDepth[GTid.x][GTid.y], max(linearMaxDepth[GTid.x + 2][GTid.y], max(linearMaxDepth[GTid.x][GTid.y + 2], linearMaxDepth[GTid.x + 2][GTid.y + 2])));
        linearMaxDepth[GTid.x][GTid.y] = maxDepth;
        texture_output_mip3[DTid.xy / 4] = maxDepth;
    }
    GroupMemoryBarrierWithGroupSync();

    if (GTid.x % 8 == 0 && GTid.y % 8 == 0)
    {
        maxDepth = max(linearMaxDepth[GTid.x][GTid.y], max(linearMaxDepth[GTid.x + 4][GTid.y], max(linearMaxDepth[GTid.x][GTid.y + 4], linearMaxDepth[GTid.x + 4][GTid.y + 4])));
        linearMaxDepth[GTid.x][GTid.y] = maxDepth;
        texture_output_mip4[DTid.xy / 8] = maxDepth;
    }
    GroupMemoryBarrierWithGroupSync();

    if (GTid.x % 16 == 0 && GTid.y % 16 == 0)
    {
        maxDepth = max(linearMaxDepth[GTid.x][GTid.y], max(linearMaxDepth[GTid.x + 8][GTid.y], max(linearMaxDepth[GTid.x][GTid.y + 8], linearMaxDepth[GTid.x + 8][GTid.y + 8])));
        texture_output_mip5[DTid.xy / 16] = maxDepth;
    }
}