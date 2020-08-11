#include "..\hf\global.hlsli"
#include "..\shaderInterop\shaderInteropParticle.h"

RWSTRUCTUREDBUFFER(shaderParticles, ShaderParticle, 0);
RWSTRUCTUREDBUFFER(particleAliveList, uint, 1);
RWSTRUCTUREDBUFFER(particleAliveListNew, uint, 2);
RWSTRUCTUREDBUFFER(particleDeadList, uint, 3);
RWRAWBUFFER(bufferCounters, 4);


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
        float3 pos = mul(gParticleWorldTransform, float4(0, 0, 0, 1.0f)).xyz;
        float size = gParticleSize;

        // create new particle
        ShaderParticle newParticle;
        newParticle.position = pos;
        newParticle.velocity = float3(Rand(seed, uv), Rand(seed, uv), Rand(seed, uv)) - 0.5f;
        newParticle.angularVelocity = 0.0f;
        newParticle.maxLife = gParticleLifeRange;
        newParticle.life = newParticle.maxLife;
        newParticle.sizeBeginEnd = float2(size, size);
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