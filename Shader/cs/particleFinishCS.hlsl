#include "..\hf\global.hlsli"
#include "..\shaderInterop\shaderInteropParticle.h"

RAWBUFFER(bufferCounters, TEXTURE_SLOT_0);
RWRAWBUFFER(bufferIndirect, 0);

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint aliveCount = bufferCounters.Load(CounterOffsetAliveAfterUpdateCount);
    
    // (VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation)
    bufferIndirect.Store4(IndirectOffsetInstanced, uint4(
        aliveCount * 6, 1, 0, 0
    ));
}