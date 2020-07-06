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
    vsOut.rtIndex = input.instance.userdata.x;
    vsOut.pos = mul(gCubeMapVP[vsOut.rtIndex], mul(surface.position, worldMat));
    
    return vsOut;
}