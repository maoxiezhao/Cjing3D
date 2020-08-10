#ifndef _SHADERINTEROP_PARTICLE_H_
#define _SHADERINTEROP_PARTICLE_H_

#include "shaderInterop.h"

struct ShaderParticle
{
	float3 position;
	float padding;
	float3 velocity;
	float angularVelocity;
	float life;
	float maxLife;
	float2 sizeBeginEnd;
};

struct ParticleCounter
{
	uint aliveCount;
	uint deadCount;
	uint emitCount;
	uint aliveAfterUpdateCount;
};

CBUFFER(ParticleCB, CBSLOT_PARTICLE)
{
	float4x4 gParticleWorldTransform;
	float gParticleEmitCount;
	float gParticleRandomness;
	float gParticleLifeRange;
	float gParticleSize;
};

#define THREAD_PARTICLE_EMIT_COUNT 256
#define THREAD_PARTICLE_UPDATE_COUNT 256

static const uint CounterOffsetAliveCount = 0;
static const uint CounterOffsetDeadCount = CounterOffsetAliveCount + 4;
static const uint CounterOffsetEmitCount = CounterOffsetDeadCount + 4;
static const uint CounterOffsetAliveAfterUpdateCount = CounterOffsetEmitCount + 4;

static const uint IndirectOffsetDispatchEmit   = 0;
static const uint IndirectOffsetDispatchUpdate = IndirectOffsetDispatchEmit + (3 * 4);
static const uint IndirectOffsetInstanced      = IndirectOffsetDispatchUpdate + (3 * 4);

#endif
