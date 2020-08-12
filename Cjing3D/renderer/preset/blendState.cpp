#include "renderPreset.h"

namespace Cjing3D {

	void RenderPreset::SetupBlendStates()
	{
		{
			BlendStateDesc desc = {};
			desc.mAlphaToCoverageEnable = false;
			desc.mIndependentBlendEnable = false;
			desc.mRenderTarget[0].mBlendEnable = false;
			desc.mRenderTarget[0].mRenderTargetWriteMask = COLOR_WRITE_ENABLE_ALL;
			desc.mRenderTarget[0].mSrcBlend = BLEND_SRC_ALPHA;
			desc.mRenderTarget[0].mDstBlend = BLEND_INV_SRC_ALPHA;
			desc.mRenderTarget[0].mBlendOp = BLEND_OP_MAX;
			desc.mRenderTarget[0].mSrcBlendAlpha = BLEND_ONE;
			desc.mRenderTarget[0].mDstBlendAlpha = BLEND_ZERO;
			desc.mRenderTarget[0].mBlendOpAlpha = BLEND_OP_ADD;
			const auto result = mDevice.CreateBlendState(desc, *mBlendStates[BlendStateID::BlendStateID_Opaque]);
			Debug::ThrowIfFailed(result, "Failed to create opaque blendState", result);
		}
		{
			BlendStateDesc desc = {};
			desc.mAlphaToCoverageEnable = false;
			desc.mIndependentBlendEnable = false;
			desc.mRenderTarget[0].mBlendEnable = false;
			desc.mRenderTarget[0].mRenderTargetWriteMask = COLOR_WRITE_DISABLE;
			const auto result = mDevice.CreateBlendState(desc, *mBlendStates[BlendStateID::BlendStateID_ColorWriteDisable]);
			Debug::ThrowIfFailed(result, "Failed to create color write disable blendState", result);
		}
		{
			BlendStateDesc desc = {};
			desc.mAlphaToCoverageEnable = false;
			desc.mIndependentBlendEnable = false;
			desc.mRenderTarget[0].mBlendEnable = true;
			desc.mRenderTarget[0].mSrcBlend = BLEND_SRC_ALPHA;
			desc.mRenderTarget[0].mDstBlend = BLEND_INV_SRC_ALPHA;
			desc.mRenderTarget[0].mBlendOp = BLEND_OP_ADD;
			desc.mRenderTarget[0].mSrcBlendAlpha = BLEND_ONE;
			desc.mRenderTarget[0].mDstBlendAlpha = BLEND_ONE;
			desc.mRenderTarget[0].mBlendOpAlpha = BLEND_OP_ADD;
			desc.mRenderTarget[0].mRenderTargetWriteMask = COLOR_WRITE_ENABLE_ALL;
			const auto result = mDevice.CreateBlendState(desc, *mBlendStates[BlendStateID::BlendStateID_Transpranent]);
			Debug::ThrowIfFailed(result, "Failed to create transparent blendState", result);
		}
		{
			// 用于2D场景渲染时使用，当渲染2D场景时，3D场景的显示以2D场景的Alpha作为Factor，BLEND_INV_SRC_ALPHA
			BlendStateDesc desc = {};
			desc.mAlphaToCoverageEnable = false;
			desc.mIndependentBlendEnable = false;
			desc.mRenderTarget[0].mBlendEnable = true;
			desc.mRenderTarget[0].mSrcBlend = BLEND_ONE;
			desc.mRenderTarget[0].mDstBlend = BLEND_INV_SRC_ALPHA;
			desc.mRenderTarget[0].mBlendOp = BLEND_OP_ADD;
			desc.mRenderTarget[0].mSrcBlendAlpha = BLEND_ONE;
			desc.mRenderTarget[0].mDstBlendAlpha = BLEND_ONE;
			desc.mRenderTarget[0].mBlendOpAlpha = BLEND_OP_ADD;
			desc.mRenderTarget[0].mRenderTargetWriteMask = COLOR_WRITE_ENABLE_ALL;
			const auto result = mDevice.CreateBlendState(desc, *mBlendStates[BlendStateID::BlendStateID_PreMultiplied]);
			Debug::ThrowIfFailed(result, "Failed to create transparent blendState", result);
		}
		{
			BlendStateDesc desc = {};
			desc.mAlphaToCoverageEnable = false;
			desc.mIndependentBlendEnable = false;
			desc.mRenderTarget[0].mBlendEnable = true;
			desc.mRenderTarget[0].mSrcBlend = BLEND_SRC_ALPHA;
			desc.mRenderTarget[0].mDstBlend = BLEND_INV_SRC_ALPHA;
			desc.mRenderTarget[0].mBlendOp = BLEND_OP_ADD;
			desc.mRenderTarget[0].mSrcBlendAlpha = BLEND_ONE;
			desc.mRenderTarget[0].mDstBlendAlpha = BLEND_INV_SRC_ALPHA;
			desc.mRenderTarget[0].mBlendOpAlpha = BLEND_OP_ADD;
			desc.mRenderTarget[0].mRenderTargetWriteMask = COLOR_WRITE_ENABLE_ALL;
			const auto result = mDevice.CreateBlendState(desc, *mBlendStates[BlendStateID::BlendStateID_Particle_Alpha]);
			Debug::ThrowIfFailed(result, "Failed to create particle alpah blendState", result);
		}
	}

}