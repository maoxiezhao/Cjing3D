#include "particle.h"
#include "shaderInterop\shaderInteropParticle.h"
#include "renderer\renderer.h"
#include "renderer\passes\particle\particlePass.h"
#include "system\sceneSystem.h"
#include "helper\random.h"

#include "helper\jsonArchive.h"
#include "helper\binaryArchive.h"

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
			desc.mUsage = USAGE_DEFAULT;
			desc.mBindFlags = BIND_UNORDERED_ACCESS | BIND_SHADER_RESOURCE;
			desc.mMiscFlags = RESOURCE_MISC_BUFFER_STRUCTURED;
			desc.mByteWidth = sizeof(ShaderParticle) * mMaxParticleCount;
			desc.mStructureByteStride = sizeof(ShaderParticle);

			const auto result = device.CreateBuffer(&desc, mBufferParticles, nullptr);
			Debug::ThrowIfFailed(result, "failed to create shader particle buffer:%08x", result);
			device.SetResourceName(mBufferParticles, "ShaderParticleArray");
		}
		/////////////////////////////////////////////////////////////////////////////////////
		{
			GPUBufferDesc desc = {};
			desc.mUsage = USAGE_DEFAULT;
			desc.mBindFlags = BIND_UNORDERED_ACCESS | BIND_SHADER_RESOURCE;
			desc.mMiscFlags = RESOURCE_MISC_BUFFER_STRUCTURED;
			desc.mByteWidth = sizeof(U32) * mMaxParticleCount;
			desc.mStructureByteStride = sizeof(U32);

			device.CreateBuffer(&desc, mBufferAliveList, nullptr);
			device.CreateBuffer(&desc, mBufferAliveListNew, nullptr);	

			std::vector<U32> deadIndices(mMaxParticleCount);
			for (int i = 0; i < mMaxParticleCount; i++) {
				deadIndices[i] = i;
			}
			SubresourceData data;
			data.mSysMem = deadIndices.data();
			device.CreateBuffer(&desc, mBufferDeadList, &data);

		}
		/////////////////////////////////////////////////////////////////////////////////////
		{
			ParticleCounter counters = {};
			counters.aliveCount = 0;
			counters.deadCount = mMaxParticleCount;
			counters.aliveAfterUpdateCount = 0;
			counters.emitCount = 0;

			GPUBufferDesc desc = {};
			desc.mUsage = USAGE_DEFAULT;
			desc.mBindFlags = BIND_UNORDERED_ACCESS | BIND_SHADER_RESOURCE;
			desc.mMiscFlags = RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
			desc.mByteWidth = sizeof(ParticleCounter);
			desc.mStructureByteStride = sizeof(ParticleCounter);

			SubresourceData data;
			data.mSysMem = &counters;

			const auto result = device.CreateBuffer(&desc, mBufferCounters, &data);
			Debug::ThrowIfFailed(result, "failed to create ParticleCounters buffer:%08x", result);
			device.SetResourceName(mBufferCounters, "ParticleCounters");
		}
		/////////////////////////////////////////////////////////////////////////////////////
		{
			GPUBufferDesc desc = {};
			desc.mUsage = USAGE_DEFAULT;
			desc.mBindFlags = BIND_CONSTANT_BUFFER;
			desc.mByteWidth = sizeof(ParticleCB);
			device.CreateBuffer(&desc, mConstantBuffer, nullptr);
		}
		/////////////////////////////////////////////////////////////////////////////////////
		{
			GPUBufferDesc desc = {};
			desc.mUsage = USAGE_DEFAULT;
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
		mConstantBuffer.Clear();
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

		// calculate emit count
		F32 fEmitCount = mEmitCountPerSec * deltaTime + mEmitWastedCount;
		I32 emitCountPerSec = (I32)std::floor(fEmitCount);
		mEmitWastedCount = fEmitCount - emitCountPerSec;

		mEmitCount = 0;
		mEmitCount += emitCountPerSec + mBurstCount;
		mBurstCount = 0;

		std::swap(mBufferAliveList, mBufferAliveListNew);

		// read particles data from GPU
		if (mCounterReadBackIndex > CounterReadBackDelayCount)
		{
			GPUResourceMapping mapping;
			mapping.mFlags = GPUResourceMapping::FLAG_READ;

			U32 index = GetDelayGPUReadBackIndex();
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
		ParticleCB particleCB;
		particleCB.gParticleWorldTransform = transform->GetWorldTransform();
		particleCB.gParticleEmitCount = (F32)mEmitCount;
		particleCB.gParticleRandomness = Random::GetRandomFloat(0, 1.0f);
		particleCB.gParticleColor = Color4::Convert(material->mBaseColor).GetRGBA();
		particleCB.gParticleLifeRange = mLife;
		particleCB.gParticleSize = mSize;
		particleCB.gParticleLifeRandomness = mRandomLife;
		particleCB.gParticleFactorRandomness = mRandomFactor;
		particleCB.gParticleSizeScaling = mSizeScaling;
		particleCB.gParticleRotation = mRotation;
		particleCB.gParticleInitialVelocity = mInitialVelocity;
		particleCB.gParticleOpacity = mOpacity;
		particleCB.gParticleMeshIndexCount = mesh != nullptr ? mesh->mIndices.size() : 0;
		particleCB.gParticleMeshVertexPosStride = sizeof(MeshComponent::VertexPosNormalSubset);

		device.UpdateBuffer(mConstantBuffer, &particleCB);
		device.BindConstantBuffer(SHADERSTAGES_CS, mConstantBuffer, CB_GETSLOT_NAME(ParticleCB));
	

		auto renderPass = Renderer::GetRenderPass(STRING_ID(ParticlePass));
		if (renderPass != nullptr)
		{
			ParticlePass& particlePass = dynamic_cast<ParticlePass&>(*renderPass);
			particlePass.UpdateParticle(*this, mesh);
		}

		U32 index = (mCounterReadBackIndex - 1) % CounterReadBackDelayCount;
		device.CopyGPUResource(mCounterReadBackBuffer[index], mBufferCounters);
	}

	void ParticleComponent::Burst(I32 count)
	{
		mBurstCount += count;
	}

	void ParticleComponent::SetMaxParticleCount(U32 count)
	{
		mMaxParticleCount = count;
		mIsRenderDataDirty = true;
	}

	U32 ParticleComponent::GetMaxParticleCount() const
	{
		return mMaxParticleCount;
	}

	void ParticleComponent::SaveToXML(const std::string& path)
	{
		JsonArchive archive(path, ArchiveMode::ArchiveMode_Write);
		// 存在Entity无法序列化问题，与Scene Entity序列化不一致
		//archive.Write("MeshID", mMeshID);
		archive.Write("StatusFlag", mStatusFlag);
		archive.Write("MaxCount", mMaxParticleCount);
		archive.Write("Life", mLife);
		archive.Write("RandomLife", mRandomLife);
		archive.Write("Size", mSize);
		archive.Write("RandomFactor", mRandomFactor);
		archive.Write("SizeScaling", mSizeScaling);
		archive.Write("Rotation", mRotation);
		archive.Write("EmitCountPerSec", mEmitCountPerSec);
		archive.Write("InitialVelocity", mInitialVelocity);
	}

	void ParticleComponent::LoadFromXML(const std::string& path)
	{
		JsonArchive archive(path, ArchiveMode::ArchiveMode_Read);
		// 存在Entity无法序列化问题，与Scene Entity序列化不一致
		//archive.Read("MeshID", mMeshID);
		archive.Read("StatusFlag", mStatusFlag);
		archive.Read("MaxCount", mMaxParticleCount);
		archive.Read("Life", mLife);
		archive.Read("RandomLife", mRandomLife);
		archive.Read("Size", mSize);
		archive.Read("RandomFactor", mRandomFactor);
		archive.Read("SizeScaling", mSizeScaling);
		archive.Read("Rotation", mRotation);
		archive.Read("EmitCountPerSec", mEmitCountPerSec);
		archive.Read("InitialVelocity", mInitialVelocity);
	}

	void ParticleComponent::Serialize(Archive& archive, U32 seed)
	{
		mMeshID = SerializeEntity(archive, seed);
		archive >> mStatusFlag;
		archive >> mMaxParticleCount;
		archive >> mLife;
		archive >> mRandomLife;
		archive >> mSize;
		archive >> mRandomFactor;
		archive >> mSizeScaling;
		archive >> mRotation;
		archive >> mEmitCountPerSec;
		archive >> mInitialVelocity;
	}

	void ParticleComponent::Unserialize(Archive& archive) const
	{
		archive << mMeshID;
		archive << mStatusFlag;
		archive << mMaxParticleCount;
		archive << mLife;
		archive << mRandomLife;
		archive << mSize;
		archive << mRandomFactor;
		archive << mSizeScaling;
		archive << mRotation;
		archive << mEmitCountPerSec;
		archive << mInitialVelocity;
	}

}