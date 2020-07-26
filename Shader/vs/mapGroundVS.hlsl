#include "..\hf\objectInput.hlsli"
#include "..\hf\shaderInteropGame.h"

inline VertexSurface GroundMakeVertexSurfaceFromInput(InputObjectAll input)
{
    VertexSurface surface;
    surface.position = float4(input.pos.xyz, 1.0f);
    surface.color = gMaterial.baseColor;
    
    if (gMaterial.useVertexColor)
    {
        surface.color = input.color;
    }

    uint normalSubsetIndex = asuint(input.pos.w);
    surface.normal.x = (float) (normalSubsetIndex & 0x000000ff) / 255.0f * 2.0f - 1.0f;
    surface.normal.y = (float) ((normalSubsetIndex >> 8) & 0x000000ff) / 255.0f * 2.0f - 1.0f;
    surface.normal.z = (float) ((normalSubsetIndex >> 16) & 0x000000ff) / 255.0f * 2.0f - 1.0f;

    surface.uv = (input.tex * input.instance.userdata.zw + input.instance.userdata.xy) * float2(gGroundTilesetInvWidth, gGroundTilesetInvHeight);

    return surface;
}


PixelInputType main(InputObjectAll input)
{
    PixelInputType Out;

    float4x4 worldMat = MakeWorldMatrixFromInstance(input.instance);
    VertexSurface surface = GroundMakeVertexSurfaceFromInput(input);

    // HLSL ����Ĭ��Ϊ�о��� mul Ĭ���������ҳ˾���
    // worldMat��C++���Ѿ����д洢������mul�ҳ�
    // gCameraVP���ǰ���Ϊ������constantBuffer���棬�������
    
	// world transform
    surface.position = mul(surface.position, worldMat);
    surface.normal = normalize(mul(surface.normal, (float3x3) worldMat));

    Out.pos = mul(gCameraVP, surface.position);
    Out.pos3D = surface.position;
    Out.nor = surface.normal;
    Out.tex = surface.uv;
    Out.color = surface.color;
    Out.pos2D = Out.pos;
    
    return Out;
}