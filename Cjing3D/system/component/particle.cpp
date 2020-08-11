#include "particle.h"
#include "shaderInterop\shaderInteropParticle.h"
#include "renderer\renderer.h"
#include "renderer\passes\particlePass.h"
#include "system\sceneSystem.h"
#include "helper\random.h"

namespace Cjing3D {

	U32 ParticleComponent::GetDelayGPUReadBackIndex() const
	{
		return (mCounterReadBackIndex + 1) % CounterReadBackDelayCount;
	}

	ParticleComponent::ParticleComponent() :
		Component(ComponentType_Particle)
	{
	}

	ParticleComponent::~ParticleComponent()
	{
	}

	void ParticleComponent::SetupRenderData()
	{
		ClearRenderData();
		GraphicsDevice& device = Renderer::GetDevice();

		/////////////////////////////////////////////////////////////////////////////////////
		{
			GPUBufferDesc desc = {};
			desc.mBindFlags = BIND_UNORDERED_ACCESS | BIND_SHADER_RESOURCE;
			desc.mMiscFlags = RESOURCE_MISC_BUFFER_STRUCTURED;
			desc.mByteWidth = sizeof(ShaderParticle) * MAX_PARTICLE_COUNT;
			desc.mStructureByteStride = sizeof(ShaderParticle);

			const auto result = device.CreateBuffer(&desc, mBufferParticles, nullptr);
			Debug::ThrowIfFailed(result, "failed to create shader particle buffer:%08x", result);
			device.SetResourceName(mBufferParticles, "ShaderParticleArray");
		}
		/////////////////////////////////////////////////////////////////////////////////////
		{
			GPUBufferDesc desc = {};
			desc.mBindFlags = BIND_UNORDERED_ACCESS | BIND_SHADER_RESOURCE;
			desc.mMiscFlags = RESOURCE_MISC_BUFFER_STRUCTURED;
			desc.mByteWidth = sizeof(U32) * MAX_PARTICLE_COUNT;
			desc.mStructureByteStride = sizeof(U32);

			device.CreateBuffer(&desc, mBufferAliveList, nullptr);
			device.CreateBuffer(&desc, mBufferAliveListNew, nullptr);	
			device.CreateBuffer(&desc, mBufferDeadList, nullptr);

		}
		/////////////////////////////////////////////////////////////////////////////////////
		{
			ParticleCounter counters = {};
			counters.aliveCount = 0;
			counters.deadCount = MAX_PARTICLE_COUNT;
			counters.aliveAfterUpdateCount = 0;
			counters.emitCount = 0;

			GPUBufferDesc desc = {};
			desc.mBindFlags = BIND_UNORDERED_ACCESS | BIND_SHADER_RESOURCE;
			desc.mMiscFlags = RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
			desc.mByteWidth = sizeof(ParticleCounter);
			desc.mStructureByteStride = sizeof(ParticleCounter);

			SubresourceData data;
			data.mSysMem = &counters;

			const auto result = device.CreateBuffer(&desc, mBufferCounters, &data);
			Debug::ThrowIfFailed(result, "failed to create ParticleCounters buffer:%08x", result);
			device.SetResourceName(mBufferParticles, "ParticleCounters");
		}
		/////////////////////////////////////////////////////////////////////////////////////
		{
			GPUBufferDesc desc = {};
			desc.mBindFlags = BIND_CONSTANT_BUFFER;
			desc.mByteWidth = sizeof(ParticleCB);
			device.CreateBuffer(&desc, mConstBuffer, nullptr);
		}
		/////////////////////////////////////////////////////////////////////////////////////
		{
			GPUBufferDesc desc = {};
			desc.mBindFlags = BIND_UNORDERED_ACCESS;
			desc.mMiscFlags = RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS | RESOURCE_MISC_DRAWINDIRECT_ARGS;
			desc.mByteWidth = 
				sizeof(IndirectArgsDispatch) +
				sizeof(IndirectArgsDispatch) +
				sizeof(IndirectArgsInstanced);
			device.CreateBuffer(&desc, mBufferIndirect, nullptr);
		}
		/////////////////////////////////////////////////////////////////////////////////////
		{
			GPUBufferDesc desc = {};
			desc.mByteWidth = sizeof(ParticleCounter);
			desc.mUsage = USAGE_STAGING;
			desc.mCPUAccessFlags = CPU_ACCESS_READ;

			for (int i = 0; i < CounterReadBackDelayCount; i++) {
				device.CreateBuffer(&desc, mCounterReadBackBuffer[i], nullptr);
			}
		}
	}

	void ParticleComponent::ClearRenderData()
	{
		mBufferParticles.Clear();
		mBufferAliveList.Clear();
		mBufferAliveListNew.Clear();
		mBufferDeadList.Clear();
		mBufferCounters.Clear();
		mConstBuffer.Clear();
		mBufferIndirect.Clear();
	}

	void ParticleComponent::UpdateCPU(F32 deltaTime)
	{
		if (IsPaused()) {
			return;
		}

		if (mIsRenderDataDirty)
		{
			mIsRenderDataDirty = false;
			SetupRenderData();
		}

		ECS::Entity selfEntity = GetCurrentEntity();
		Scene& scene = Scene::GetScene();
		auto transform = scene.mTransforms.GetComponent(selfEntity);
		mPosition = XMConvert(transform->GetWorldPosition());

		mEmitCount = 0;
		mEmitCount += mBurstCount;
		mBurstCount = 0;

		// read particles data from GPU
		if (mCounterReadBackIndex > CounterReadBackDelayCount)
		{
			U32 index = GetDelayGPUReadBackIndex();
			GPUResourceMapping mapping;
			Renderer::GetDevice().Map(&mCounterReadBackBuffer[index], mapping);
			if (mapping.mData != nullptr)
			{
				Memory::Memcpy(&mParticleCounter, mapping.mData, sizeof(ParticleCounter));
				Renderer::GetDevice().Unmap(&mCounterReadBackBuffer[index]);
			}
		}
		mCounterReadBackIndex++;
	}

	void ParticleComponent::UpdateGPU()
	{
		if (IsPaused()) {
			return;
		}

		ECS::Entity selfEntity = GetCurrentEntity();
		Scene& scene = Scene::GetScene();
		auto transform = scene.mTransforms.GetComponent(selfEntity);
		auto material = scene.mMaterials.GetComponent(selfEntity);
		auto mesh = scene.mMeshes.GetComponent(mMeshID);
	
		GraphicsDevice &device = Renderer::GetDevice();
		// update particle buffer
		{
			ParticleCB particleCB;
			particleCB.gParticleWorldTransform = transform->GetWorldTransform();
			particleCB.gParticleEmitCount = (F32)mEmitCount;
			particleCB.gParticleRandomness = Random::GetRandomFloat(0, 1.0f);
			particleCB.gParticleColor = Color4::Convert(material->mBaseColor).GetRGBA();

			device.UpdateBuffer(mConstBuffer, &particleCB);
			device.BindConstantBuffer(SHADERSTAGES_CS, mConstBuffer, CB_GETSLOT_NAME(ParticleCB));
		}

		auto renderPass = Renderer::GetRenderPass(STRING_ID(TerrainPass));
		if (renderPass != nullptr)
		{
			ParticlePass& particlePass = dynamic_cast<ParticlePass&>(*renderPass);
			particlePass.UpdateParticle(*this);
		}

		U32 index = (mCounterReadBackIndex - 1) % CounterReadBackDelayCount;
		device.CopyGPUResource(mCounterReadBackBuffer[index], mBufferCounters);
	}

	void ParticleComponent::Burst(I32 count)
	{
		mBurstCount += count;
	}

	void ParticleComponent::Serialize(Archive& archive, U32 seed)
	{
	}

	void ParticleComponent::Unserialize(Archive& archive) const
	{
	}

}