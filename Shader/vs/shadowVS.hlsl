#include "..\hf\objectHF.hlsli"

float4 main(InputObjectPos input) : SV_POSITION
{
    float4x4 worldMat = MakeWorldMatrixFromInstance(input.instance);
    VertexSurface surface = MakeVertexSurfaceFromInput(input);

	// world transform
    float4 output;
    output = mul(worldMat, surface.position);
    output = mul(gCameraVP, output);
    
    return output;
}