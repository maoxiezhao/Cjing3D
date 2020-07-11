#include "..\hf\objectHF.hlsli"

struct VSOutType
{
    float4 pos   : SV_POSITION;
    uint rtIndex : SV_RenderTargetArrayIndex;
};

VSOutType main(InputObjectPos input)
{
    float4x4 worldMat = MakeWorldMatrixFromInstance(input.instance);
    VertexSurface surface = MakeVertexSurfaceFromInput(input);

    VSOutType vsOut;
    CubemapRenderCamera cam = gCubemapRenderCams[input.instance.userdata.x];
    vsOut.rtIndex = cam.properties.x;
    vsOut.pos = mul(cam.cubemapVP, mul(surface.position, worldMat));
    
    return vsOut;
}