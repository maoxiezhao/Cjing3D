#include "PipelineStateManager.h"
#include "renderer\shaderLib.h"
#include "renderer\preset\renderPreset.h"

namespace Cjing3D {
namespace {
	U32 ConvertStringID(const StringID& id)
	{
		U32 hash = id.HashValue();
		return hash < 100 ? hash + 100 : hash;
	}

}

PipelineStateManager::PipelineStateManager()
{
}

PipelineStateManager::~PipelineStateManager()
{
}

void PipelineStateManager::SetupPipelineStateIDs()
{
	RenderPreset& renderPreset = Renderer::GetRenderPreset();
	{
		PipelineStateDesc desc = {};
		desc.mVertexShader      = renderPreset.GetVertexShader(VertexShaderType_Sky);
		desc.mPixelShader       = renderPreset.GetPixelShader(PixelShaderType_Sky);
		desc.mBlendState	    = renderPreset.GetBlendState(BlendStateID_Opaque);
		desc.mDepthStencilState = renderPreset.GetDepthStencilState(DepthStencilStateID_DepthRead);
		desc.mRasterizerState   = renderPreset.GetRasterizerState(RasterizerStateID_Sky);
		desc.mPrimitiveTopology = TRIANGLESTRIP;

		RegisterPipelineState(PipelineStateID_SkyRendering, desc);
	}
	{
		PipelineStateDesc desc  = {};
		desc.mVertexShader      = renderPreset.GetVertexShader(VertexShaderType_PosColor);
		desc.mPixelShader	    = renderPreset.GetPixelShader(PixelShaderType_PosColor);
		desc.mInputLayout	    = renderPreset.GetVertexLayout(InputLayoutType_PosColor);
		desc.mBlendState	    = renderPreset.GetBlendState(BlendStateID_Transpranent);
		desc.mDepthStencilState = renderPreset.GetDepthStencilState(DepthStencilStateID_DepthRead);
		desc.mRasterizerState   = renderPreset.GetRasterizerState(RasterizerStateID_WireFrame_DoubleSided);
		desc.mPrimitiveTopology = LINELIST;

		RegisterPipelineState(PipelineStateID_GridHelper, desc);
	}
}

PipelineState* PipelineStateManager::GetPipelineStateByID(PipelineStateID stateID)
{
	auto it = mPipelineStateIDMap.find(stateID);
	if (it != mPipelineStateIDMap.end())
	{
		return &mPipelineStates[it->second];
	}
	else
	{
		return nullptr;
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

PipelineState* PipelineStateManager::GetCustomPipelineState(RenderPassType passType, const StringID& id)
{
	auto& passPSOs = mCustomPipelineStateMap.mPassPSOs[passType];
	auto stateID = ConvertStringID(id);
	auto it = passPSOs.mPSONameMap.find(stateID);
	if (it != passPSOs.mPSONameMap.end())
	{
		return &mPipelineStates[it->second];
	}
	else
	{
		return nullptr;
	}
}

void PipelineStateManager::RegisterCustomPipelineState(RenderPassType passType, const StringID& name, PipelineStateDesc desc)
{
	auto& passPSOs = mCustomPipelineStateMap.mPassPSOs[passType];
	auto stateID = ConvertStringID(name);
	auto it = passPSOs.mPSONameMap.find(stateID);
	if (it == passPSOs.mPSONameMap.end())
	{
		U32 index = RegisterPipelineState(desc);
		passPSOs.mPSONameMap[stateID] = index;
	}
}

}