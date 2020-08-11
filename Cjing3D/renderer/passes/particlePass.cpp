#include "particlePass.h"
#include "system\sceneSystem.h"
#include "resource\resourceManager.h"
#include "renderer\renderer.h"
#include "renderer\preset\renderPreset.h"

namespace Cjing3D {
namespace {
	PipelineState mParticlePSO;
	ShaderPtr mParticleStartCS  = nullptr;
	ShaderPtr mParticleEmitCS = nullptr;
	ShaderPtr mParticleUpdateCS = nullptr;
	ShaderPtr mParticleFinishCS = nullptr;
}

ParticlePass::ParticlePass()
{
}

ParticlePass::~ParticlePass()
{
}

void ParticlePass::Initialize()
{
	ShaderLib& shaderLib = Renderer::GetShaderLib();
	RenderPreset& renderPreset = Renderer::GetRenderPreset();
	ResourceManager& resourceManager = GlobalGetSubSystem<ResourceManager>();

	const std::string shaderPath = resourceManager.GetStandardResourceDirectory(Resource_Shader);
	PipelineStateDesc desc = {};
	desc.mVertexShader = Renderer::LoadShader(SHADERSTAGES_VS, shaderPath + "particleVS.cso");
	desc.mPixelShader = Renderer::LoadShader(SHADERSTAGES_PS, shaderPath + "particlePS.cso");
	desc.mPrimitiveTopology = TRIANGLELIST;
	desc.mBlendState = renderPreset.GetBlendState(BlendStateID_Transpranent);
	desc.mDepthStencilState = renderPreset.GetDepthStencilState(DepthStencilStateID_DepthRead);
	desc.mRasterizerState = renderPreset.GetRasterizerState(RasterizerStateID_Particle);

	Renderer::GetDevice().CreatePipelineState(desc, mParticlePSO);

	// load compute shaders
	mParticleStartCS  = Renderer::LoadShader(SHADERSTAGES_CS, shaderPath + "particleStartCS.cso");
	mParticleEmitCS   = Renderer::LoadShader(SHADERSTAGES_CS, shaderPath + "particleEmitCS.cso");
	mParticleUpdateCS = Renderer::LoadShader(SHADERSTAGES_CS, shaderPath + "particleUpdateCS.cso");
	mParticleFinishCS = Renderer::LoadShader(SHADERSTAGES_CS, shaderPath + "particleFinishCS.cso");
}

void ParticlePass::Uninitialize()
{
	mParticleStartCS->Clear();
	mParticleEmitCS->Clear();
	mParticleUpdateCS->Clear();
	mParticleFinishCS->Clear();

	mParticleStartCS = nullptr;
	mParticleEmitCS = nullptr;
	mParticleUpdateCS = nullptr;
	mParticleFinishCS = nullptr;

	mParticlePSO.Clear();
}

void ParticlePass::UpdateParticle(ParticleComponent& particle)
{
	GraphicsDevice& device = Renderer::GetDevice();
	GPUResource* uavs[] = {
		&particle.mBufferParticles,
		&particle.mBufferAliveList,
		&particle.mBufferAliveListNew,
		&particle.mBufferDeadList,
		&particle.mBufferCounters,
		&particle.mBufferIndirect
	};
	device.BindUAVs(uavs, 0, ARRAYSIZE(uavs));
	// start 
	{
		device.BeginEvent("ParticleStart");
		device.BindComputeShader(mParticleStartCS);
		device.Dispatch(1, 1, 1);
		device.EndEvent();
	}
	// emit
	{
		device.BeginEvent("ParticleEmit");
		device.BindComputeShader(mParticleEmitCS);
		device.DispatchIndirect(&particle.mBufferIndirect, IndirectOffsetDispatchEmit);
		device.EndEvent();
	}
	// update
	{
		device.BeginEvent("ParticleUpdate");
		device.BindComputeShader(mParticleUpdateCS);
		device.DispatchIndirect(&particle.mBufferIndirect, IndirectOffsetDispatchUpdate);
		device.EndEvent();
	}
	device.UnBindUAVs(0, ARRAYSIZE(uavs));

	// finish
	{
		device.BeginEvent("ParticleFinish");
		device.BindUAV(&particle.mBufferIndirect, 0);
		device.BindComputeShader(mParticleFinishCS);
		device.BindGPUResource(SHADERSTAGES_CS, &particle.mBufferCounters, TEXTURE_SLOT_0);
		device.Dispatch(1, 1, 1);
		device.UnBindUAVs(0, 1);
		device.UnbindGPUResources(TEXTURE_SLOT_0, 1);
		device.EndEvent();
	}
}

void ParticlePass::DrawParticle(ParticleComponent& particle, MaterialComponent& material)
{
	GraphicsDevice &device = Renderer::GetDevice();
	device.BeginEvent("RenderParticles");
	device.BindPipelineState(mParticlePSO);
	device.BindGPUResource(SHADERSTAGES_PS, material.GetBaseColorMap(), TEXTURE_SLOT_0);
	device.BindConstantBuffer(SHADERSTAGES_VS, particle.mConstBuffer, CB_GETSLOT_NAME(ParticleCB));
	device.BindConstantBuffer(SHADERSTAGES_PS, particle.mConstBuffer, CB_GETSLOT_NAME(ParticleCB));
	
	GPUResource* resources[] = {
		&particle.mBufferParticles,
		&particle.mBufferAliveList,
	};
	device.BindGPUResources(SHADERSTAGES_VS, resources, TEXTURE_SLOT_8, ARRAYSIZE(resources));
	device.DrawInstancedIndirect(&particle.mBufferIndirect, IndirectOffsetInstanced);
	device.EndEvent();
}

PipelineState& ParticlePass::GetPipelineState()
{
	return mParticlePSO;
}

}