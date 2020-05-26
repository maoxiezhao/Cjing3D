#include "..\hf\objectHF.hlsli"

PixelInputTypePosTexColor main(InputObjectPosTex input)
{
    float4x4 worldMat = MakeWorldMatrixFromInstance(input.instance);
    VertexSurface surface = MakeVertexSurfaceFromInput(input);

    PixelInputTypePosTexColor Out;
    // world transform
    surface.position = mul(surface.position, worldMat);

    Out.pos = mul(gCameraVP, surface.position);
    Out.tex = surface.uv;
    Out.color = surface.color;
    
    return Out;
}