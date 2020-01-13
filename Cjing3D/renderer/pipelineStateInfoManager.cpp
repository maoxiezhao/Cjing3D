#include "pipelineStateInfoManager.h"
#include "shaderLib.h"
#include "stateManager.h"

namespace Cjing3D {

PipelineStateInfoManager::PipelineStateInfoManager(Renderer & renderer):
	mRenderer(renderer)
{
}

PipelineStateInfoManager::~PipelineStateInfoManager()
{
}

void PipelineStateInfoManager::SetupPipelineStateInfos()
{
}

PipelineStateInfo PipelineStateInfoManager::GetPipelineStateInfo(ShaderType shaderType, MaterialComponent & material)
{
	ShaderLib& shaderLib = mRenderer.GetShaderLib();
	StateManager& stateManager = mRenderer.GetStateManager();

	PipelineStateInfo infoState;
	infoState.Register(&mRenderer.GetDevice());

	if (shaderType == ShaderType_Forward)
	{
		infoState.mVertexShader = shaderLib.GetVertexShader(VertexShaderType_Transform);
		infoState.mPixelShader = shaderLib.GetPixelShader(PixelShaderType_Forward);
		infoState.mInputLayout = shaderLib.GetVertexLayout(InputLayoutType_Transform);
		infoState.mPrimitiveTopology = TRIANGLELIST;
		infoState.mBlendState = stateManager.GetBlendState(BlendStateID_Opaque);
		infoState.mDepthStencilState = stateManager.GetDepthStencilState(DepthStencilStateID_GreaterEqualReadWrite);
		infoState.mRasterizerState = stateManager.GetRasterizerState(RasterizerStateID_Front);
	}

	return infoState;
}

PipelineStateInfo PipelineStateInfoManager::GetImagePipelineStateInfo(RenderImage::ImageParams params)
{
	ShaderLib& shaderLib = mRenderer.GetShaderLib();
	StateManager& stateManager = mRenderer.GetStateManager();

	PipelineStateInfo infoState;
	infoState.Register(&mRenderer.GetDevice());

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

	return infoState;
}
}