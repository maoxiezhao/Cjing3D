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

PipelineStateInfo PipelineStateInfoManager::GetImagePipelineStateInfo()
{
	ShaderLib& shaderLib = mRenderer.GetShaderLib();
	StateManager& stateManager = mRenderer.GetStateManager();

	PipelineStateInfo infoState;
	infoState.Register(&mRenderer.GetDevice());

	infoState.mVertexShader = shaderLib.GetVertexShader(VertexShaderType_FullScreen);
	infoState.mPixelShader = shaderLib.GetPixelShader(PixelShaderType_FullScreen);
	infoState.mPrimitiveTopology = TRIANGLELIST;

	return infoState;
}
}