#include "..\hf\objectHF.hlsli"

struct VertexToPixel
{
    float4 pos   : SV_Position;
    float4 color : COLOR;
};

VertexToPixel main(float4 pos : POSITION, float4 color : TEXCOORD0)
{ 
    VertexToPixel output;
    output.pos = mul(gMiscTransform, pos);
    output.color = color * gMiscColor;
    return output;
}