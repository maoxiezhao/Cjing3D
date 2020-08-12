#include "..\hf\global.hlsli"
#include "..\shaderInterop\shaderInteropParticle.h"

RWSTRUCTUREDBUFFER(shaderParticles, ShaderParticle, 0);
RWSTRUCTUREDBUFFER(particleAliveList, uint, 1);
RWSTRUCTUREDBUFFER(particleAliveListNew, uint, 2);
RWSTRUCTUREDBUFFER(particleDeadList, uint, 3);
RWRAWBUFFER(bufferCounters, 4);
RWRAWBUFFER(bufferIndirect, 5);

[numthreads(THREAD_PARTICLE_UPDATE_COUNT, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint aliveCount = bufferCounters.Load(CounterOffsetAliveCount);
    if (DTid.x < aliveCount)
    {
        uint particleIndex = particleAliveList[DTid.x];
        ShaderParticle particle = shaderParticles[particleIndex];
        if (particle.life <= 0)
        {
            uint deadIndex;
            bufferCounters.InterlockedAdd(CounterOffsetDeadCount, 1, deadIndex);
            particleDeadList[deadIndex] = particleIndex;
        }
        else
        {
            const float dt = gFrameDeltaTime;
            particle.position += particle.velocity * dt;    
            particle.life = particle.life - dt;
            
            shaderParticles[particleIndex] = particle;
            
            // 为了不影响other threads读取aliveCount,这里将Update后的粒子数量写入到
            // counter.AliveAfterUpdateCount中
            uint newIndex;
            bufferCounters.InterlockedAdd(CounterOffsetAliveAfterUpdateCount, 1, newIndex);
            particleAliveListNew[newIndex] = particleIndex;
        }
    }
   
}