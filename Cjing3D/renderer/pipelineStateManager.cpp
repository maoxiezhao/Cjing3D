#include "PipelineStateManager.h"
#include "shaderLib.h"
#include "stateManager.h"

namespace Cjing3D {

namespace{
	U32 ConvertImageParamsToHashCode(RenderImage::ImageParams params)
	{
		U32 hashValue = 0;
		HashCombine(hashValue, params.IsFullScreenEnabled() ? 1 : 0);
		HashCombine(hashValue, static_cast<U32>(params.mBlendType));
		return hashValue;
	}

	U32 ConvertNormalRenderParamsToHashCode(NormalRenderParams params)
	{
		U32 hashValue = 0;
		HashCombine(hashValue, static_cast<U32>(params.renderPassType));
		return hashValue;
	}
}

PipelineStateManager::PipelineStateManager(Renderer & renderer):
	mRenderer(renderer)
{
}

PipelineStateManager::~PipelineStateManager()
{
}

void PipelineStateManager::SetupNormalPipelineStates()
{
	ShaderLib& shaderLib = mRenderer.GetShaderLib();
	StateManager& stateManager = mRenderer.GetStateManager();
	NormalRenderParams params = {};

	// base object rendering
	{
		params.renderPassType = RenderPassType_Forward;

		PipelineStateDesc infoState;
		infoState.mVertexShader = shaderLib.GetVertexShader(VertexShaderType_Transform);
		infoState.mPixelShader = shaderLib.GetPixelShader(PixelShaderType_Forward);
		infoState.mInputLayout = shaderLib.GetVertexLayout(InputLayoutType_Transform);
		infoState.mPrimitiveTopology = TRIANGLELIST;
		infoState.mBlendState = stateManager.GetBlendState(BlendStateID_Opaque);
		infoState.mDepthStencilState = stateManager.GetDepthStencilState(DepthStencilStateID_GreaterEqualReadWrite);
		infoState.mRasterizerState = stateManager.GetRasterizerState(RasterizerStateID_Front);

		RegisterPipelineState(params, infoState);
	}

	// directional light shadow map
	{
		params.renderPassType = RenderPassType_Shadow;

		PipelineStateDesc infoState;
		infoState.mVertexShader = shaderLib.GetVertexShader(VertexShaderType_Shadow);
		infoState.mPixelShader = nullptr;  // only write depth
		infoState.mInputLayout = shaderLib.GetVertexLayout(InputLayoutType_Shadow);
		infoState.mPrimitiveTopology = TRIANGLELIST;
		infoState.mBlendState = stateManager.GetBlendState(BlendStateID_ColorWriteDisable);
		infoState.mDepthStencilState = stateManager.GetDepthStencilState(DepthStencilStateID_Shadow);
		infoState.mRasterizerState = stateManager.GetRasterizerState( RasterizerStateID_Shadow);

		RegisterPipelineState(params, infoState);
	}
}

void PipelineStateManager::SetupImagePipelineStates()
{
	ShaderLib& shaderLib = mRenderer.GetShaderLib();
	StateManager& stateManager = mRenderer.GetStateManager();
	RenderImage::ImageParams params = {};

	for (U32 blendTypeIndex = 0; blendTypeIndex < BlendType::BlendType_Count; blendTypeIndex++)
	{
		BlendType blendType = static_cast<BlendType>(blendTypeIndex);
		params.EnableFullScreen();
		params.mBlendType = blendType;

		PipelineStateDesc infoState;
		infoState.mPrimitiveTopology = TRIANGLESTRIP;

		if (params.IsFullScreenEnabled())
		{
			infoState.mVertexShader = shaderLib.GetVertexShader(VertexShaderType_FullScreen);
			infoState.mPixelShader = shaderLib.GetPixelShader(PixelShaderType_FullScreen);
		}
		else
		{
			infoState.mVertexShader = shaderLib.GetVertexShader(VertexShaderType_Image);
			infoState.mPixelShader = shaderLib.GetPixelShader(PixelShaderType_Image);
			infoState.mDepthStencilState = stateManager.GetDepthStencilState(DepthStencilStateID_DepthNone);
			infoState.mRasterizerState = stateManager.GetRasterizerState(RasterizerStateID_Image);
		}

		if (params.mBlendType == BlendType_Opaque) {
			infoState.mBlendState = stateManager.GetBlendState(BlendStateID_Opaque);
		}
		else if (params.mBlendType == BlendType_Alpha) {
			infoState.mBlendState = stateManager.GetBlendState(BlendStateID_Transpranent);
		}
		else {
			infoState.mBlendState = stateManager.GetBlendState(BlendStateID_PreMultiplied);
		}

		RegisterPipelineState(params, infoState);
	}
}

void PipelineStateManager::SetupPipelineStateIDs()
{
	ShaderLib& shaderLib = mRenderer.GetShaderLib();
	StateManager& stateManager = mRenderer.GetStateManager();
	{
		PipelineStateDesc desc = {};
		desc.mVertexShader      = shaderLib.GetVertexShader(VertexShaderType_Sky);
		desc.mPixelShader       = shaderLib.GetPixelShader(PixelShaderType_Sky);
		desc.mBlendState	    = stateManager.GetBlendState(BlendStateID_Opaque);
		desc.mDepthStencilState = stateManager.GetDepthStencilState(DepthStencilStateID_DepthRead);
		desc.mRasterizerState   = stateManager.GetRasterizerState(RasterizerStateID_Sky);
		desc.mPrimitiveTopology = TRIANGLESTRIP;

		RegisterPipelineState(PipelineStateID_SkyRendering, desc);
	}
}

PipelineState PipelineStateManager::GetNormalPipelineState(RenderPassType renderPassType, MaterialComponent & material)
{
	NormalRenderParams params;
	params.renderPassType = renderPassType;

	U32 hashValue = ConvertNormalRenderParamsToHashCode(params);
	auto it = mNormalPipelineStateIndexMap.find(hashValue);
	if (it != mNormalPipelineStateIndexMap.end()) {
		return mPipelineStates[it->second];
	}
	else {
		return PipelineState();
	}
}

PipelineState PipelineStateManager::GetImagePipelineState(RenderImage::ImageParams params)
{
	U32 hashValue = ConvertImageParamsToHashCode(params);
	auto it = mImagePipelineStateIndexMap.find(hashValue);
	if (it != mImagePipelineStateIndexMap.end()) {
		return mPipelineStates[it->second];
	}
	else {
		return PipelineState();
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
	mRenderer.GetDevice().CreatePipelineState(desc, state);

	return index;
}

void PipelineStateManager::RegisterPipelineState(RenderImage::ImageParams params, PipelineStateDesc desc)
{
	U32 hashValue = ConvertImageParamsToHashCode(params);
	auto it = mImagePipelineStateIndexMap.find(hashValue);
	if (it == mImagePipelineStateIndexMap.end()) 
	{
		U32 index = RegisterPipelineState(desc);
		mImagePipelineStateIndexMap[hashValue] = index;
	}
}

void PipelineStateManager::RegisterPipelineState(NormalRenderParams params, PipelineStateDesc desc)
{
	U32 hashValue = ConvertNormalRenderParamsToHashCode(params);
	auto it = mNormalPipelineStateIndexMap.find(hashValue);
	if (it == mNormalPipelineStateIndexMap.end())
	{
		U32 index = RegisterPipelineState(desc);
		mNormalPipelineStateIndexMap[hashValue] = index;
	}
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