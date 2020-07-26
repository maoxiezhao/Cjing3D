#include "PipelineStateManager.h"
#include "renderer\shaderLib.h"
#include "renderer\preset\renderPreset.h"
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

		InputLayoutPtr GetInputLayout(RenderPassType renderPass, bool alphaTest, RenderPreset& renderPreset)
		{
			InputLayoutPtr ret = nullptr;
			switch (renderPass)
			{
			case RenderPassType_Forward:
			case RenderPassType_TiledForward:
				ret = renderPreset.GetVertexLayout(InputLayoutType_ObjectAll);
				break;
			case RenderPassType_Shadow:
			case RenderPassType_ShadowCube:
				ret = renderPreset.GetVertexLayout(InputLayoutType_Shadow);
				break;
			case RenderPassType_Depth:
				ret = alphaTest == true ?
					renderPreset.GetVertexLayout(InputLayoutType_ObjectPosTex) :
					renderPreset.GetVertexLayout(InputLayoutType_ObjectPos);
				break;
			default:
				break;
			}
			return ret;
		}

		ShaderPtr GetVertexShader(RenderPassType renderPass, bool alphaTest, RenderPreset& renderPreset)
		{
			ShaderPtr ret = nullptr;
			switch (renderPass)
			{
			case RenderPassType_Forward:
			case RenderPassType_TiledForward:
				ret = renderPreset.GetVertexShader(VertexShaderType_ObjectAll);
				break;
			case RenderPassType_Shadow:
				ret = renderPreset.GetVertexShader(VertexShaderType_Shadow);
				break;
			case RenderPassType_ShadowCube:
				ret = renderPreset.GetVertexShader(VertexShaderType_ShadowCube);
				break;
			case RenderPassType_Depth:
				ret = alphaTest == true ?
					renderPreset.GetVertexShader(VertexShaderType_ObjectPosTex) :
					renderPreset.GetVertexShader(VertexShaderType_ObjectPos);
				break;
			default:
				break;
			}
			return ret;
		}

		ShaderPtr GetPixelShader(RenderPassType renderPass, bool alphaTest, bool transparent, RenderPreset& renderPreset)
		{
			ShaderPtr ret = nullptr;
			switch (renderPass)
			{
			case RenderPassType_Forward:
				if (transparent) {
					ret = renderPreset.GetPixelShader(PixelShaderType_Object_Forward_Transparent);
				} else {
					ret = renderPreset.GetPixelShader(PixelShaderType_Object_Forward);
				}
				break;
			case RenderPassType_TiledForward:
				if (transparent) {
					ret = renderPreset.GetPixelShader(PixelShaderType_Object_TiledForward_Transparent);
				}
				else {
					ret = renderPreset.GetPixelShader(PixelShaderType_Object_TiledForward);
				}
				break;
			case RenderPassType_Depth:
				ret = alphaTest == true ?
					renderPreset.GetPixelShader(PixelShaderType_Object_AlphaTest) : nullptr;
				break;
			default:
				break;
			}
			return ret;
		}
	}

	void PipelineStateManager::SetupNormalPipelineStates()
	{
		RenderPreset& renderPreset = Renderer::GetRenderPreset();
		NormalRenderParams params = {};

		// base object rendering
		const std::vector<RenderPassType> renderPasses = 
		{ 
			RenderPassType_Forward, 
			RenderPassType_TiledForward,
			RenderPassType_Depth, 
			RenderPassType_Shadow,
			RenderPassType_ShadowCube
		};
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
					infoState.mVertexShader = GetVertexShader(renderPass, alphaTest, renderPreset);
					infoState.mPixelShader = GetPixelShader(renderPass, alphaTest, transparent, renderPreset);
					infoState.mInputLayout = GetInputLayout(renderPass, alphaTest, renderPreset);

					// blendState
					switch (blendType)
					{
					case BlendType_Opaque:
						infoState.mBlendState = renderPreset.GetBlendState(BlendStateID_Opaque);
						break;
					case BlendType_Alpha:
						infoState.mBlendState = renderPreset.GetBlendState(BlendStateID_Transpranent);
						break;
					case BlendType_PreMultiplied:
						infoState.mBlendState = renderPreset.GetBlendState(BlendStateID_PreMultiplied);
						break;
					default:
						assert(0);
						break;
					}
					if (!transparent && 
						renderPass == RenderPassType_Depth && 
						renderPass == RenderPassType_Shadow &&
						renderPass == RenderPassType_ShadowCube) {
						infoState.mBlendState = renderPreset.GetBlendState(BlendStateID_ColorWriteDisable);
					}

					// depthStencilState
					switch (renderPass)
					{
					case RenderPassType_Forward:
					case RenderPassType_TiledForward:
						if (transparent)
						{
							infoState.mDepthStencilState = renderPreset.GetDepthStencilState(DepthStencilStateID_GreaterEqualReadWrite);
						}
						else
						{
							// 因为非透明物体在depth prepass中已经写入了depthBuffer(支持alphaTest，减少overdraw)，
							// 这里只需要读取depthBuffer，且相等时绘制
							infoState.mDepthStencilState = renderPreset.GetDepthStencilState(DepthStencilStateID_DepthReadEqual);
						}
						break;
					case RenderPassType_Shadow:
					case RenderPassType_ShadowCube:
						infoState.mDepthStencilState = renderPreset.GetDepthStencilState(DepthStencilStateID_Shadow);
						break;
					default:
						infoState.mDepthStencilState = renderPreset.GetDepthStencilState(DepthStencilStateID_GreaterEqualReadWrite);
						break;
					}

					// rasteriazer
					switch (renderPass)
					{
					case RenderPassType_Shadow:
					case RenderPassType_ShadowCube:
						infoState.mRasterizerState = renderPreset.GetRasterizerState(RasterizerStateID_Shadow);
						break;
					default:
						infoState.mRasterizerState = renderPreset.GetRasterizerState(RasterizerStateID_Front);
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


	PipelineState* PipelineStateManager::GetNormalPipelineState(RenderPassType renderPassType, MaterialComponent& material, bool forceAlphaTest)
	{
		NormalRenderParams params;
		params.renderPassType = renderPassType;
		params.blendType = material.GetBlendMode();
		params.enableAlphaTest = material.IsNeedAlphaTest();

		U32 hashValue = ConvertNormalRenderParamsToHashCode(params);
		auto it = mNormalPipelineStateIndexMap.find(hashValue);
		if (it != mNormalPipelineStateIndexMap.end()) {
			return &mPipelineStates[it->second];
		}
		else {
			return nullptr;
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