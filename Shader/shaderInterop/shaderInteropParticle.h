#ifndef _SHADERINTEROP_PARTICLE_H_
#define _SHADERINTEROP_PARTICLE_H_

#include "shaderInterop.h"

struct ShaderParticle
{
	float3 position;
	uint color;
	float3 velocity;
	float angularVelocity;
	float2 sizeBeginEnd;
	float maxLife;
	float life;
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
	float gParticleLifeRandomness;
	float gParticleSize;
	float gParticleSizeScaling;
	uint  gParticleColor;
	float gParticleFactorRandomness;
	float gParticleRotation;
	float gParticleInitialVelocity;
	float gParticleOpacity;
	uint  gParticleMeshIndexCount;
	uint  gParticleMeshVertexPosStride;
	float3 gParticlePadding;
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
