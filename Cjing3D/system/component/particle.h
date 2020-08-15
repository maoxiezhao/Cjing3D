#pragma once

#include "system\component\componentInclude.h"
#include "shaderInterop\shaderInteropParticle.h"

namespace Cjing3D {

	class CameraComponent;

	class ParticleComponent : public Component
	{
	public:
		GPUBuffer mBufferParticles;
		GPUBuffer mBufferAliveList;
		GPUBuffer mBufferAliveListNew;
		GPUBuffer mBufferDeadList;
		GPUBuffer mBufferCounters;
		GPUBuffer mConstBuffer;
		GPUBuffer mBufferIndirect;

		// particle attributes
		ECS::Entity mMeshID = ECS::INVALID_ENTITY;

		enum ParticleFlag
		{
			EMPTY = 0,
			PAUSED = 1,
		};
		U32 mStatusFlag = EMPTY;
		U32 MAX_PARTICLE_COUNT = 1000;
		F32 mLife = 1.0f;
		F32 mRandomLife = 1.0f;
		F32 mSize = 1.0f;

		// run time attributes
		bool mIsRenderDataDirty = true;
		F32x3 mPosition = F32x3(0.0f, 0.0f, 0.0f);
		I32 mBurstCount = 0;
		I32 mEmitCount = 0;

	private:
		// 用于延迟读取GPU端返回数据
		ParticleCounter mParticleCounter;
		U32 mCounterReadBackIndex = 0;
		static const size_t CounterReadBackDelayCount = 4;
		GPUBuffer mCounterReadBackBuffer[CounterReadBackDelayCount];

		U32 GetDelayGPUReadBackIndex()const;

	public:
		ParticleComponent();
		~ParticleComponent();

		void SetupRenderData();
		void ClearRenderData();
		void UpdateCPU(F32 deltaTime);
		void UpdateGPU();
		void Burst(I32 count);

		void SetPaused(bool isPaused) { if (isPaused) { mStatusFlag |= ParticleFlag::PAUSED; } else { mStatusFlag &= ~ParticleFlag::PAUSED;} }
		bool IsPaused()const { return mStatusFlag & ParticleFlag::PAUSED; };
		void SetMaxParticleCount(U32 count);
		U32 GetMaxParticleCount()const;

		// xml serialize
		void SaveToXML(const std::string& path);
		void LoadFromXML(const std::string& path);

		// scene binary serialize
		virtual void Serialize(Archive& archive, U32 seed = 0);
		virtual void Unserialize(Archive& archive)const;
	};

}