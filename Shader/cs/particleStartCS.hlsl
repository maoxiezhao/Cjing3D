#include "..\hf\global.hlsli"
#include "..\shaderInterop\shaderInteropParticle.h"

RWRAWBUFFER(bufferCounters, 4);
RWRAWBUFFER(bufferIndirect, 5);

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint deadCount = bufferCounters.Load(CounterOffsetDeadCount);
    uint realEmitCount = min(deadCount, gParticleEmitCount);
    uint newAliveCount = bufferCounters.Load(CounterOffsetAliveAfterUpdateCount);
    
    // write indirect buffer
    bufferIndirect.Store3(IndirectOffsetDispatchEmit,   uint3(ceil((float) realEmitCount   / (float)THREAD_PARTICLE_EMIT_COUNT), 1, 1));
    bufferIndirect.Store3(IndirectOffsetDispatchUpdate, uint3(ceil((float) (newAliveCount + realEmitCount) / (float) THREAD_PARTICLE_UPDATE_COUNT), 1, 1));
    
    // update counter
    bufferCounters.Store(CounterOffsetAliveCount, newAliveCount);
    bufferCounters.Store(CounterOffsetAliveAfterUpdateCount, 0);
    bufferCounters.Store(CounterOffsetEmitCount, realEmitCount);
}