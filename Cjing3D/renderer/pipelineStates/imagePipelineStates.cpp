#include "PipelineStateManager.h"
#include "renderer\shaderLib.h"
#include "renderer\preset\renderPreset.h"

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
		ShaderLib& shaderLib = Renderer::GetShaderLib();
		RenderPreset& renderPreset = Renderer::GetRenderPreset();
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
				infoState.mVertexShader = renderPreset.GetVertexShader(VertexShaderType_FullScreen);
				infoState.mPixelShader = renderPreset.GetPixelShader(PixelShaderType_FullScreen);
			}
			else
			{
				infoState.mVertexShader = renderPreset.GetVertexShader(VertexShaderType_Image);
				infoState.mPixelShader = renderPreset.GetPixelShader(PixelShaderType_Image);
				infoState.mDepthStencilState = renderPreset.GetDepthStencilState(DepthStencilStateID_DepthNone);
				infoState.mRasterizerState = renderPreset.GetRasterizerState(RasterizerStateID_Image);
			}

			if (params.mBlendType == BlendType_Opaque) {
				infoState.mBlendState = renderPreset.GetBlendState(BlendStateID_Opaque);
			}
			else if (params.mBlendType == BlendType_Alpha) {
				infoState.mBlendState = renderPreset.GetBlendState(BlendStateID_Transpranent);
			}
			else {
				infoState.mBlendState = renderPreset.GetBlendState(BlendStateID_PreMultiplied);
			}

			RegisterPipelineState(params, infoState);
		}
	}

	PipelineState* PipelineStateManager::GetImagePipelineState(RenderImage::ImageParams params)
	{
		U32 hashValue = ConvertImageParamsToHashCode(params);
		auto it = mImagePipelineStateIndexMap.find(hashValue);
		if (it != mImagePipelineStateIndexMap.end()) {
			return &mPipelineStates[it->second];
		}
		else {
			return nullptr;
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