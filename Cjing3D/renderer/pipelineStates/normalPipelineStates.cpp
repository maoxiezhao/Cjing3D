#include "PipelineStateManager.h"
#include "renderer\shaderLib.h"
#include "renderer\stateManager.h"
#include "system\component\material.h"

namespace Cjing3D
{
	namespace
	{
		U32 ConvertNormalRenderParamsToHashCode(NormalRenderParams params)
		{
			U32 hashValue = 0;
			HashCombine(hashValue, static_cast<U32>(params.renderPassType));
			HashCombine(hashValue, static_cast<U32>(params.blendType));
			return hashValue;
		}

		VertexShaderPtr GetVertexShader(RenderPassType renderPass, ShaderLib& shaderLib)
		{
			VertexShaderPtr ret = nullptr;
			switch (renderPass)
			{
			case RenderPassType_Forward:
				ret = shaderLib.GetVertexShader(VertexShaderType_Transform);
				break;
			case RenderPassType_Deferred:
				break;
			case RenderPassType_Shadow:
				ret = shaderLib.GetVertexShader(VertexShaderType_Shadow);
				break;
			default:
				break;
			}
			return ret;
		}

		PixelShaderPtr GetPixelShader(RenderPassType renderPass, bool transparent, ShaderLib& shaderLib)
		{
			PixelShaderPtr ret = nullptr;
			switch (renderPass)
			{
			case RenderPassType_Forward:
				if (transparent) {
					ret = shaderLib.GetPixelShader(PixelShaderType_Forward_Transparent);
				} else {
					ret = shaderLib.GetPixelShader(PixelShaderType_Forward);
				}
				break;
			case RenderPassType_Deferred:
				break;
			case RenderPassType_Shadow:
				break;
			default:
				break;
			}
			return ret;
		}
	}


	void PipelineStateManager::SetupNormalPipelineStates()
	{
		ShaderLib& shaderLib = mRenderer.GetShaderLib();
		StateManager& stateManager = mRenderer.GetStateManager();
		NormalRenderParams params = {};

		// base object rendering
		{
			RenderPassType renderPass = RenderPassType_Forward;
			for (int blendTypeIndex = 0; blendTypeIndex < BlendType::BlendType_Count; blendTypeIndex++)
			{
				BlendType blendType = static_cast<BlendType>(blendTypeIndex);
				bool transparent = blendType != BlendType::BlendType_Opaque;

				PipelineStateDesc infoState;
				// shader
				infoState.mPrimitiveTopology = TRIANGLELIST;
				infoState.mVertexShader = GetVertexShader(renderPass, shaderLib);
				infoState.mPixelShader = GetPixelShader(renderPass, transparent, shaderLib);
				infoState.mInputLayout = shaderLib.GetVertexLayout(InputLayoutType_Transform);

				// blendState
				switch (blendType)
				{
				case BlendType_Opaque:
					infoState.mBlendState = stateManager.GetBlendState(BlendStateID_Opaque);
					break;
				case BlendType_Alpha:
					infoState.mBlendState = stateManager.GetBlendState(BlendStateID_Transpranent);
					break;
				case BlendType_PreMultiplied:
					infoState.mBlendState = stateManager.GetBlendState(BlendStateID_PreMultiplied);
					break;
				default:
					assert(0);
					break;
				}

				infoState.mDepthStencilState = stateManager.GetDepthStencilState(DepthStencilStateID_GreaterEqualReadWrite);
				infoState.mRasterizerState = stateManager.GetRasterizerState(RasterizerStateID_Front);

				params.renderPassType = renderPass;
				params.blendType = blendType;
				RegisterPipelineState(params, infoState);
			}
		}

		// directional light shadow map
		{
			RenderPassType renderPass = RenderPassType_Shadow;
			PipelineStateDesc infoState;
			infoState.mVertexShader = GetVertexShader(renderPass, shaderLib);
			infoState.mPixelShader = nullptr;  // only write depth
			infoState.mInputLayout = shaderLib.GetVertexLayout(InputLayoutType_Shadow);
			infoState.mPrimitiveTopology = TRIANGLELIST;
			infoState.mBlendState = stateManager.GetBlendState(BlendStateID_ColorWriteDisable);
			infoState.mDepthStencilState = stateManager.GetDepthStencilState(DepthStencilStateID_Shadow);
			infoState.mRasterizerState = stateManager.GetRasterizerState(RasterizerStateID_Shadow);

			params.renderPassType = renderPass;
			params.blendType = BlendType_Opaque;
			RegisterPipelineState(params, infoState);
		}
	}


	PipelineState PipelineStateManager::GetNormalPipelineState(RenderPassType renderPassType, MaterialComponent& material, bool forceAlphaTest)
	{
		NormalRenderParams params;
		params.renderPassType = renderPassType;
		params.blendType = material.GetBlendMode();

		U32 hashValue = ConvertNormalRenderParamsToHashCode(params);
		auto it = mNormalPipelineStateIndexMap.find(hashValue);
		if (it != mNormalPipelineStateIndexMap.end()) {
			return mPipelineStates[it->second];
		}
		else {
			return PipelineState();
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
}