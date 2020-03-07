#include "..\hf\imageHF.hlsli"

ImageVertexToPixel main(uint vertexID : SV_VERTEXID)
{
    //	1--2
	//	  /
	//	 /
	//	3--4
    ImageVertexToPixel Out;
    
    Out.pos = gImageCorners[vertexID];
    Out.uv = float2(vertexID % 2, vertexID % 4 / 2);
    
	return Out;
}