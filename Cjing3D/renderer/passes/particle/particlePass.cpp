#include "particlePass.h"
#include "system\sceneSystem.h"
#include "resource\resourceManager.h"
#include "renderer\renderer.h"
#include "renderer\preset\renderPreset.h"

namespace Cjing3D {
namespace {
	PipelineState mParticleAlphaPSO;

	ShaderPtr mParticleStartCS  = nullptr;
	ShaderPtr mParticleEmitCS = nullptr;
	ShaderPtr mParticleEmitMeshCS = nullptr;
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

	// create pipeline state object
	PipelineStateDesc desc = {};
	desc.mVertexShader = Renderer::LoadShader(SHADERSTAGES_VS, "particleVS.cso");
	desc.mPixelShader  = Renderer::LoadShader(SHADERSTAGES_PS, "particlePS.cso");
	desc.mPrimitiveTopology = TRIANGLELIST;
	desc.mBlendState = renderPreset.GetBlendState(BlendStateID_Particle_Alpha);
	desc.mDepthStencilState = renderPreset.GetDepthStencilState(DepthStencilStateID_DepthRead);
	desc.mRasterizerState = renderPreset.GetRasterizerState(RasterizerStateID_Particle);
	Renderer::GetDevice().CreatePipelineState(desc, mParticleAlphaPSO);

	// load compute shaders
	mParticleStartCS    = Renderer::LoadShader(SHADERSTAGES_CS, "particleStartCS.cso");
	mParticleEmitCS     = Renderer::LoadShader(SHADERSTAGES_CS, "particleEmitCS.cso");
	mParticleEmitMeshCS = Renderer::LoadShader(SHADERSTAGES_CS, "particleEmitMeshCS.cso");
	mParticleUpdateCS   = Renderer::LoadShader(SHADERSTAGES_CS, "particleUpdateCS.cso");
	mParticleFinishCS   = Renderer::LoadShader(SHADERSTAGES_CS, "particleFinishCS.cso");
}

void ParticlePass::Uninitialize()
{
	mParticleStartCS->Clear();
	mParticleEmitCS->Clear();
	mParticleEmitMeshCS->Clear();
	mParticleUpdateCS->Clear();
	mParticleFinishCS->Clear();

	mParticleStartCS = nullptr;
	mParticleEmitCS = nullptr;
	mParticleEmitMeshCS = nullptr;
	mParticleUpdateCS = nullptr;
	mParticleFinishCS = nullptr;

	mParticleAlphaPSO.Clear();
}

void ParticlePass::UpdateParticle(CommandList cmd, ParticleComponent& particle, MeshComponent* mesh)
{
	GraphicsDevice& device = Renderer::GetDevice();
	device.BeginEvent(cmd, "ParticleUpdate");

	GPUResource* uavs[] = {
		&particle.mBufferParticles,
		&particle.mBufferAliveList,
		&particle.mBufferAliveListNew,
		&particle.mBufferDeadList,
		&particle.mBufferCounters,
		&particle.mBufferIndirect
	};
	device.BindUAVs(cmd, uavs, 0, ARRAYSIZE(uavs));

	GPUResource* meshRes[] = {
		mesh != nullptr ? mesh->GetFinalVertexBufferPos() : nullptr,
		mesh != nullptr ? mesh->GetIndexBuffer() : nullptr
	};
	device.BindGPUResources(cmd, SHADERSTAGES_CS, meshRes, TEXTURE_SLOT_0, ARRAYSIZE(meshRes));

	// start 
	device.BeginEvent(cmd, "ParticleStart");
	device.BindComputeShader(cmd, mParticleStartCS);
	device.Dispatch(cmd, 1, 1, 1);
	device.EndEvent(cmd);
	// emit
	device.BeginEvent(cmd, "ParticleEmit");
	device.BindComputeShader(cmd, mesh != nullptr ? mParticleEmitMeshCS : mParticleEmitCS);
	device.DispatchIndirect(cmd, &particle.mBufferIndirect, IndirectOffsetDispatchEmit);
	device.EndEvent(cmd);
	// update
	device.BeginEvent(cmd, "ParticleUpdate");
	device.BindComputeShader(cmd, mParticleUpdateCS);
	device.DispatchIndirect(cmd, &particle.mBufferIndirect, IndirectOffsetDispatchUpdate);
	device.EndEvent(cmd);
	device.UnBindUAVs(cmd, 0, ARRAYSIZE(uavs));

	device.UnbindGPUResources(cmd, TEXTURE_SLOT_0, ARRAYSIZE(meshRes));

	// finish
	device.BeginEvent(cmd, "ParticleFinish");
	device.BindUAV(cmd, &particle.mBufferIndirect, 0);
	device.BindComputeShader(cmd, mParticleFinishCS);
	device.BindGPUResource(cmd, SHADERSTAGES_CS, &particle.mBufferCounters, TEXTURE_SLOT_0);
	device.Dispatch(cmd, 1, 1, 1);
	device.UnBindUAVs(cmd, 0, 1);
	device.UnbindGPUResources(cmd, TEXTURE_SLOT_0, 1);
	device.EndEvent(cmd);

	device.EndEvent(cmd);
}

void ParticlePass::DrawParticle(CommandList cmd, ParticleComponent& particle, MaterialComponent& material)
{
	GraphicsDevice &device = Renderer::GetDevice();
	device.BeginEvent(cmd, "RenderParticles");
	switch (material.GetBlendMode())
	{
	case BlendType_Alpha:
		device.BindPipelineState(cmd, &mParticleAlphaPSO);
		break;
	default:
		Debug::Warning("Invalid particle material blend mode");
		device.EndEvent(cmd);
		return;
		break;
	}

	device.BindGPUResource(cmd, SHADERSTAGES_PS, material.GetBaseColorMap(), TEXTURE_SLOT_0);
	device.BindConstantBuffer(cmd, SHADERSTAGES_VS, particle.mConstantBuffer, CB_GETSLOT_NAME(ParticleCB));
	device.BindConstantBuffer(cmd, SHADERSTAGES_PS, particle.mConstantBuffer, CB_GETSLOT_NAME(ParticleCB));
	
	GPUResource* resources[] = {
		&particle.mBufferParticles,
		&particle.mBufferAliveListNew,
	};
	device.BindGPUResources(cmd, SHADERSTAGES_VS, resources, TEXTURE_SLOT_8, ARRAYSIZE(resources));
	device.DrawInstancedIndirect(cmd, &particle.mBufferIndirect, IndirectOffsetInstanced);
	device.EndEvent(cmd);
}

}