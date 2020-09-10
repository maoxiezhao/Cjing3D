#include "..\hf\global.hlsli"
#include "..\shaderInterop\shaderInteropParticle.h"

RWSTRUCTUREDBUFFER(shaderParticles, ShaderParticle, 0);
RWSTRUCTUREDBUFFER(particleAliveList, uint, 1);
RWSTRUCTUREDBUFFER(particleAliveListNew, uint, 2);
RWSTRUCTUREDBUFFER(particleDeadList, uint, 3);
RWRAWBUFFER(bufferCounters, 4);

#ifdef EMIT_FROM_MESH
RAWBUFFER(meshVertex, TEXTURE_SLOT_0);
TYPEDBUFFER(meshIndex, uint, TEXTURE_SLOT_1);
#endif

// https://www.shadertoy.com/view/llGSzw
float hash1(uint n)
{
    // integer hash copied from Hugo Elias
    n = (n << 13U) ^ n;
    n = n * (n * n * 15731U + 789221U) + 1376312589U;
    return float(n & 0x7fffffffU) / float(0x7fffffff);
}

[numthreads(THREAD_PARTICLE_EMIT_COUNT, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    // 创建新的粒子，并更新AliveList和deadList
    // deadList根据deadCount记录了可用的粒子的索引，每当创建一个新的粒子
    // 从deadList从后往前选取一个索引，deadCount--,同时AliveCount++,
    // aliveList记录了新粒子的索引（用于之后粒子死亡时，再反向填充到deadList中）
    
    uint emitCount = bufferCounters.Load(CounterOffsetEmitCount);
    if (DTid.x < emitCount)
    {
        float seed = 0.125f;
        float2 uv = float2(gFrameTime + gParticleRandomness, float(DTid.x) / (float) THREAD_PARTICLE_EMIT_COUNT);
        
#ifdef EMIT_FROM_MESH
        // 将DTid.x hash 映射到对应的三角形，但是未考虑到三角形面积，所以是不均匀的
        // TODO
        // https://www.zhihu.com/question/316940395
        uint triIndex = (uint)((gParticleMeshIndexCount / 3) * hash1(DTid.x));
       
        float4 posNor0 = asfloat(meshVertex.Load4(meshIndex[triIndex * 3 + 0] * gParticleMeshVertexPosStride));
        float4 posNor1 = asfloat(meshVertex.Load4(meshIndex[triIndex * 3 + 1] * gParticleMeshVertexPosStride));
        float4 posNor2 = asfloat(meshVertex.Load4(meshIndex[triIndex * 3 + 2] * gParticleMeshVertexPosStride));
        
        float3 nor0 = GetNormalFromNumber(asuint(posNor0.w));
        float3 nor1 = GetNormalFromNumber(asuint(posNor1.w));
        float3 nor2 = GetNormalFromNumber(asuint(posNor2.w));
        
        // 从三角形中随机取一个点
        float f = Rand(seed, uv);
        float g = Rand(seed, uv);
        [flatten]
        if (f + g > 1) 
        {
            f = 1 - f;
            g = 1 - g;
        }
        
        float3 pos = posNor0.xyz + (posNor1.xyz - posNor0.xyz) * f + (posNor2.xyz - posNor0.xyz) * g;
        float3 nor = nor0 + (nor1 - nor0) * f + (nor2 - nor0) * g;
        
        pos = mul(gParticleWorldTransform, float4(pos, 1.0f)).xyz;
        nor = normalize(mul((float3x3)gParticleWorldTransform, nor));
#else
        float3 pos = mul(gParticleWorldTransform, float4(0, 0, 0, 1.0f)).xyz;
        float3 nor = float3(0.0f, 0.0f, 0.0f);
#endif   
        float size = gParticleSize + gParticleSize * (Rand(seed, uv) - 0.5f) * gParticleFactorRandomness;

        // create new particle
        ShaderParticle newParticle;
        newParticle.position = pos;
        newParticle.velocity = (nor + (float3(Rand(seed, uv), Rand(seed, uv), Rand(seed, uv)) - 0.5f) * gParticleFactorRandomness) * gParticleInitialVelocity;
        newParticle.angularVelocity = gParticleRotation + gParticleRotation * (Rand(seed, uv) - 0.5f) * gParticleFactorRandomness;
        newParticle.maxLife = gParticleLifeRange + gParticleLifeRange * (Rand(seed, uv) - 0.5f) * gParticleLifeRandomness;
        newParticle.life = newParticle.maxLife;
        newParticle.sizeBeginEnd = float2(size, size * gParticleSizeScaling);
        newParticle.color = gParticleColor;

        // store new particle and update lists
        uint deadCount = 0;
        bufferCounters.InterlockedAdd(CounterOffsetDeadCount, -1, deadCount);
        uint newParticleIndex = particleDeadList[deadCount - 1];
        shaderParticles[newParticleIndex] = newParticle;
        
        uint aliveCount = 0;
        bufferCounters.InterlockedAdd(CounterOffsetAliveCount, 1, aliveCount);
        particleAliveList[aliveCount] = newParticleIndex;
    }
}