#ifndef _TILED_CULLING_HF_
#define _TILED_CULLING_HF_

#include "global.hlsli"

struct TiledFrustum
{
    Plane planes[4];
};

float4 ClipToView(float4 clip)
{
    float4 view = mul(gCameraInvP, clip);
    view = view / view.w;
    return view;
}

// 将屏幕坐标转换为相机裁剪空间xy<(-1, 1)
// 再从裁剪空间转换到相机空间
float4 ScreenToView(float4 screen)
{
    float2 tex = screen.xy * gFrameInvScreenSize;
    float4 clip = float4(float2(tex.x, 1.0f - tex.y) * 2.0f - 1.0f, screen.z, screen.w);
    return ClipToView(clip);
}

struct Sphere
{
    float3 center;
    float radius;
};

bool CheckSphereInsidePlane(Sphere sphere, Plane plane)
{
    return !(dot(plane.normal, sphere.center) - plane.distance < -sphere.radius);
}

bool CheckSphereInsideFrustum(Sphere sphere, TiledFrustum frustum)
{
    bool result = true;
    result = CheckSphereInsidePlane(sphere, frustum.planes[0]) ? result : false;
    result = CheckSphereInsidePlane(sphere, frustum.planes[1]) ? result : false;
    result = CheckSphereInsidePlane(sphere, frustum.planes[2]) ? result : false;
    result = CheckSphereInsidePlane(sphere, frustum.planes[3]) ? result : false;
    return result;
}

bool CheckSphereInsideFrustum(Sphere sphere, TiledFrustum frustum, float zNear, float zFar)
{
    bool result = true;
    result = (sphere.center.z + sphere.radius < zNear || sphere.center.z - sphere.radius > zFar) ? false : result;
    result = CheckSphereInsidePlane(sphere, frustum.planes[0]) ? result : false;
    result = CheckSphereInsidePlane(sphere, frustum.planes[1]) ? result : false;
    result = CheckSphereInsidePlane(sphere, frustum.planes[2]) ? result : false;
    result = CheckSphereInsidePlane(sphere, frustum.planes[3]) ? result : false;
    return result;
}

#endif

