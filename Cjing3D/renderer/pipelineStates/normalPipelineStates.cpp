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
			HashCombine(hashValue, static_cast<U32>(params.enableAlphaTest));
			return hashValue;
		}

		InputLayoutPtr GetInputLayout(RenderPassType renderPass, bool alphaTest, ShaderLib& shaderLib)
		{
			InputLayoutPtr ret = nullptr;
			switch (renderPass)
			{
			case RenderPassType_Forward:
				ret = shaderLib.GetVertexLayout(InputLayoutType_ObjectAll);
				break;
			case RenderPassType_Shadow:
				ret = shaderLib.GetVertexLayout(InputLayoutType_Shadow);
				break;
			case RenderPassType_Depth:
				ret = alphaTest == true ?
					shaderLib.GetVertexLayout(InputLayoutType_ObjectPosTex) :
					shaderLib.GetVertexLayout(InputLayoutType_ObjectPos);
				break;
			default:
				break;
			}
			return ret;
		}

		ShaderPtr GetVertexShader(RenderPassType renderPass, bool alphaTest, ShaderLib& shaderLib)
		{
			ShaderPtr ret = nullptr;
			switch (renderPass)
			{
			case RenderPassType_Forward:
				ret = shaderLib.GetVertexShader(VertexShaderType_ObjectAll);
				break;
			case RenderPassType_Shadow:
				ret = shaderLib.GetVertexShader(VertexShaderType_Shadow);
				break;
			case RenderPassType_Depth:
				ret = alphaTest == true ?
					shaderLib.GetVertexShader(VertexShaderType_ObjectPosTex) :
					shaderLib.GetVertexShader(VertexShaderType_ObjectPos);
				break;
			default:
				break;
			}
			return ret;
		}

		ShaderPtr GetPixelShader(RenderPassType renderPass, bool alphaTest, bool transparent, ShaderLib& shaderLib)
		{
			ShaderPtr ret = nullptr;
			switch (renderPass)
			{
			case RenderPassType_Forward:
				if (transparent) {
					ret = shaderLib.GetPixelShader(PixelShaderType_Object_Forward_Transparent);
				} else {
					ret = shaderLib.GetPixelShader(PixelShaderType_Object_Forward);
				}
				break;
			case RenderPassType_Depth:
				ret = alphaTest == true ?
					shaderLib.GetPixelShader(PixelShaderType_Object_AlphaTest) : nullptr;
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
		std::vector<RenderPassType> renderPasses = { RenderPassType_Forward , RenderPassType_Depth, RenderPassType_Shadow };
		for (RenderPassType renderPass : renderPasses)
		{
			for (int blendTypeIndex = 0; blendTypeIndex < BlendType::BlendType_Count; blendTypeIndex++)
			{
				for (int alphaTestIndex = 0; alphaTestIndex <= 1; alphaTestIndex++)
				{
					BlendType blendType = static_cast<BlendType>(blendTypeIndex);
					bool transparent = blendType != BlendType::BlendType_Opaque;
					bool alphaTest = alphaTestIndex > 0;

					PipelineStateDesc infoState;
					infoState.mPrimitiveTopology = TRIANGLELIST;
					infoState.mVertexShader = GetVertexShader(renderPass, alphaTest, shaderLib);
					infoState.mPixelShader = GetPixelShader(renderPass, alphaTest, transparent, shaderLib);
					infoState.mInputLayout = GetInputLayout(renderPass, alphaTest, shaderLib);

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
					if (!transparent && renderPass == RenderPassType_Depth && renderPass == RenderPassType_Shadow) {
						infoState.mBlendState = stateManager.GetBlendState(BlendStateID_ColorWriteDisable);
					}

					// depthStencilState
					switch (renderPass)
					{
					case RenderPassType_Forward:
						if (transparent)
						{
							infoState.mDepthStencilState = stateManager.GetDepthStencilState(DepthStencilStateID_GreaterEqualReadWrite);
						}
						else
						{
							// ��Ϊ��͸��������depth prepass���Ѿ�д����depthBuffer(֧��alphaTest������overdraw)��
							// ����ֻ��Ҫ��ȡdepthBuffer�������ʱ����
							infoState.mDepthStencilState = stateManager.GetDepthStencilState(DepthStencilStateID_DepthReadEqual);
						}
						break;
					case RenderPassType_Shadow:
						infoState.mDepthStencilState = stateManager.GetDepthStencilState(DepthStencilStateID_Shadow);
						break;
					default:
						infoState.mDepthStencilState = stateManager.GetDepthStencilState(DepthStencilStateID_GreaterEqualReadWrite);
						break;
					}

					// rasteriazer
					switch (renderPass)
					{
					case RenderPassType_Shadow:
						infoState.mRasterizerState = stateManager.GetRasterizerState(RasterizerStateID_Shadow);
						break;
					default:
						infoState.mRasterizerState = stateManager.GetRasterizerState(RasterizerStateID_Front);
						break;
					}
	
					params.renderPassType = renderPass;
					params.blendType = blendType;
					params.enableAlphaTest = alphaTest;
					RegisterPipelineState(params, infoState);
				}
			}
		}
	}


	PipelineState PipelineStateManager::GetNormalPipelineState(RenderPassType renderPassType, MaterialComponent& material, bool forceAlphaTest)
	{
		NormalRenderParams params;
		params.renderPassType = renderPassType;
		params.blendType = material.GetBlendMode();
		params.enableAlphaTest = material.IsNeedAlphaTest();

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