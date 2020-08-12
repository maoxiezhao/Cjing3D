#include "..\hf\particleHF.hlsli"

STRUCTUREDBUFFER(shaderParticles, ShaderParticle, TEXTURE_SLOT_8);
STRUCTUREDBUFFER(particleAliveList, uint, TEXTURE_SLOT_9);

static const float3 BillBoardQuads[] =
{
    float3(-1, -1, 0), // 0
	float3(1, -1, 0), // 1
	float3(-1, 1, 0), // 2
	float3(-1, 1, 0), // 3
	float3(1, -1, 0), // 4
	float3(1, 1, 0), // 5
};

VertexToPixel main(uint vertexID : SV_VERTEXID)
{
    // 根据当前生命计算各个属性插值
    ShaderParticle particle = shaderParticles[particleAliveList[vertexID / 6]];
    float lifeLerp = 1.0f - particle.life / particle.maxLife;
    float realSize = lerp(particle.sizeBeginEnd.x, particle.sizeBeginEnd.y, lifeLerp);
    float realOpacity = saturate(lerp(1, 0, lifeLerp));
    float rotation = lifeLerp * particle.angularVelocity;
    
    // get quad pos
    float3 quadPos = BillBoardQuads[vertexID % 6];
    float2 uv = quadPos.xy * float2(0.5f, -0.5f) + 0.5f;
    quadPos *= realSize;
    
    // rotation
    float2x2 rotMat = float2x2(
        cos(rotation), -sin(rotation),
        sin(rotation), cos(rotation)
    );
    quadPos.xy = mul(quadPos.xy, rotMat);
    
    // calculate final pos
    float4 finalPos = float4(particle.position, 1.0f);
    finalPos = mul(gCameraView, finalPos);
    finalPos.xyz += quadPos;
    finalPos = mul(gCameraProj, finalPos);
    
    VertexToPixel output;
    output.pos = finalPos;
    output.pos2D = finalPos;
    output.pos3D = mul(gCameraInvV, finalPos);
    output.tex = uv;
    output.color = (particle.color & 0x00FFFFFF) | (uint(realOpacity * 255.0f) << 24);
    
    return output;
}