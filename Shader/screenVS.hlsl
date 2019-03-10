#include "hf\fullScreenHF.hlsli"

FullScreenPixelInput main(uint vertexID : SV_VERTEXID)
{
    FullScreenPixelInput Out;

    FullScreenTriangle(vertexID, Out.pos);
    FullScreenTriangleTex(vertexID, Out.tex);

	return Out;
}