#include "..\hf\fullScreenHF.hlsli"

struct SkyVSOut
{
    float4 pos : SV_Position;
    float2 tex : TEXCOORD0;
};

SkyVSOut main(uint vertexID : SV_VERTEXID)
{
    SkyVSOut Out;

    FullScreenTriangle(vertexID, Out.pos);
    Out.tex = Out.pos.xy;

    return Out;
}