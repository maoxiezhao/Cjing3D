#include "PipelineStateManager.h"
#include "renderer\shaderLib.h"
#include "renderer\preset\renderPreset.h"

namespace Cjing3D {

PipelineStateManager::PipelineStateManager()
{
}

PipelineStateManager::~PipelineStateManager()
{
}

void PipelineStateManager::SetupPipelineStateIDs()
{
	ShaderLib& shaderLib = Renderer::GetShaderLib();
	RenderPreset& renderPreset = Renderer::GetRenderPreset();
	{
		PipelineStateDesc desc = {};
		desc.mVertexShader      = shaderLib.GetVertexShader(VertexShaderType_Sky);
		desc.mPixelShader       = shaderLib.GetPixelShader(PixelShaderType_Sky);
		desc.mBlendState	    = renderPreset.GetBlendState(BlendStateID_Opaque);
		desc.mDepthStencilState = renderPreset.GetDepthStencilState(DepthStencilStateID_DepthRead);
		desc.mRasterizerState   = renderPreset.GetRasterizerState(RasterizerStateID_Sky);
		desc.mPrimitiveTopology = TRIANGLESTRIP;

		RegisterPipelineState(PipelineStateID_SkyRendering, desc);
	}
}

PipelineState PipelineStateManager::GetPipelineStateByID(PipelineStateID stateID)
{
	auto it = mPipelineStateIDMap.find(stateID);
	if (it != mPipelineStateIDMap.end())
	{
		return mPipelineStates[it->second];
	}
	else
	{
		return PipelineState();
	}
}

void PipelineStateManager::Initialize()
{
	SetupNormalPipelineStates();
	SetupImagePipelineStates();
	SetupPipelineStateIDs();
}

void PipelineStateManager::Uninitialize()
{
	mNormalPipelineStateIndexMap.clear();
	mImagePipelineStateIndexMap.clear();
	mPipelineStates.clear();
}

U32 PipelineStateManager::RegisterPipelineState(PipelineStateDesc desc)
{
	U32 index = mPipelineStates.size();
	auto& state = mPipelineStates.emplace_back();
	Renderer::GetDevice().CreatePipelineState(desc, state);

	return index;
}

void PipelineStateManager::RegisterPipelineState(PipelineStateID stateID, PipelineStateDesc desc)
{
	auto it = mPipelineStateIDMap.find(stateID);
	if (it == mPipelineStateIDMap.end())
	{
		U32 index = RegisterPipelineState(desc);
		mPipelineStateIDMap[stateID] = index;
	}
}
}