#ifndef _TILED_CULLING_HF_
#define _TILED_CULLING_HF_

#include "global.hlsli"

struct TiledFrustum
{
    Plane planes[4];
};

// 将屏幕坐标转换为相机裁剪空间xy<(-1, 1)
float4 ScreenToView(float4 screen)
{
    float2 tex = screen.xy * gFrameInvScreenSize;
    return float4(float2(tex.x, 1.0f - tex.y) * 2.0f - 1.0f, screen.z, screen.w);

}

#endif

