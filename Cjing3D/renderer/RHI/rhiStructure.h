#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\RHI\rhiDefinition.h"

namespace Cjing3D
{
	// 视口定义
	struct ViewPort
	{
		F32 mTopLeftX;
		F32 mTopLeftY;
		F32 mWidth;
		F32 mHeight;
		F32 mMinDepth;
		F32 mMaxDepth;
	};

	// DepthStencil op desc
	struct DepthStencilOpDesc
	{
		StencilOp mStencilFailOp;
		StencilOp mStencilDepthFailOp;
		StencilOp mStencilPassOp;
		ComparisonFunc mDepthFunc;

		DepthStencilOpDesc() :
			mStencilFailOp(StencilOp::STENCIL_OP_KEEP),
			mStencilDepthFailOp(StencilOp::STENCIL_OP_KEEP),
			mStencilPassOp(StencilOp::STENCIL_OP_KEEP),
			mDepthFunc(ComparisonFunc::COMPARISON_NEVER)
		{}
	};

	struct DepthStencilStateDesc
	{
		bool mDepthEnable;
		DepthWriteMask mDepthWriteMask;
		ComparisonFunc mDepthFunc;
		bool mStencilEnable;
		U8 mStencilReadMask;
		U8 mStencilWriteMask;

		DepthStencilOpDesc mFrontFace;
		DepthStencilOpDesc mBackFace;

		DepthStencilStateDesc():
			mDepthEnable(false),
			mDepthWriteMask(DepthWriteMask::DEPTH_WRITE_MASK_ZERO),
			mDepthFunc(ComparisonFunc::COMPARISON_NEVER),
			mStencilEnable(false),
			mStencilReadMask(0xff),
			mStencilWriteMask(0xff),
			mFrontFace(),
			mBackFace()
		{}
	};

	struct BlendTargetBlendStateDesc
	{
		bool mBlendEnable;
		Blend mSrcBlend;
		Blend mDstBlend;
		BlendOp mBlendOp;

		Blend mSrcBlendAlpha;
		Blend mDstBlendAlpha;
		BlendOp mBlendOpAlpha;

		U32 mRenderTargetWriteMask;
	};

	struct BlendStateDesc
	{
		bool mAlphaToCoverageEnable;
		bool mIndependentBlendEnable;
		BlendTargetBlendStateDesc mRenderTarget[8];

		BlendStateDesc() :
			mAlphaToCoverageEnable(false),
			mIndependentBlendEnable(false)
		{}
	};

	struct RasterizerStateDesc
	{
		FillMode mFillMode;
		CullMode mCullMode;
		bool mFrontCounterClockwise;
		U32 mDepthBias;
		F32 mDepthBiasClamp;
		F32 mSlopeScaleDepthBias;
		bool mDepthClipEnable;
		bool mMultisampleEnable;
		bool mAntialiaseLineEnable;
		bool mConservativeRasterizationEnable;
		U32 mForcedSampleCount;

		RasterizerStateDesc() :
			mFillMode(FILL_SOLID),
			mCullMode(CULL_NONE),
			mFrontCounterClockwise(false),
			mDepthBias(0),
			mDepthBiasClamp(0),
			mSlopeScaleDepthBias(0.0f),
			mDepthClipEnable(false),
			mMultisampleEnable(false),
			mAntialiaseLineEnable(false),
			mConservativeRasterizationEnable(false),
			mForcedSampleCount(0)
		{}

	};

	struct VertexLayoutDesc
	{
		const char* mSemanticName;
		U32 mSemanticIndex;
		FORMAT mFormat;
		U32 mInputSlot;
		U32 mAlignedByteOffset;
		InputClassification mInputSlotClass;
		U32 mInstanceDataStepRate;
	};

}