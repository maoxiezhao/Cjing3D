#include "PipelineStateManager.h"
#include "renderer\shaderLib.h"
#include "renderer\stateManager.h"

namespace Cjing3D
{
	namespace {
		U32 ConvertImageParamsToHashCode(RenderImage::ImageParams params)
		{
			U32 hashValue = 0;
			HashCombine(hashValue, params.IsFullScreenEnabled() ? 1 : 0);
			HashCombine(hashValue, static_cast<U32>(params.mBlendType));
			return hashValue;
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
}