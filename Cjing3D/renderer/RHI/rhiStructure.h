#pragma once

#include "renderer\RHI\rhiDefinition.h"

// TODO 整合到rhiDefinition中

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

		ViewPort() :
			mTopLeftX(0.0f),
			mTopLeftY(0.0f),
			mWidth(0.0f),
			mHeight(0.0f),
			mMinDepth(0.0f),
			mMaxDepth(1.0f)
		{}
	};

	// DepthStencil op desc
	struct DepthStencilOpDesc
	{
		StencilOp mStencilFailOp;
		StencilOp mStencilDepthFailOp;
		StencilOp mStencilPassOp;
		ComparisonFunc mStencilFunc;

		DepthStencilOpDesc() :
			mStencilFailOp(StencilOp::STENCIL_OP_KEEP),
			mStencilDepthFailOp(StencilOp::STENCIL_OP_KEEP),
			mStencilPassOp(StencilOp::STENCIL_OP_KEEP),
			mStencilFunc(ComparisonFunc::COMPARISON_NEVER)
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
		U32 mSemanticIndex;				// semanticName相同时，使用的index下标（name0, name1)
		FORMAT mFormat;
		U32 mInputSlot;
		U32 mAlignedByteOffset;
		InputClassification mInputSlotClass;
		U32 mInstanceDataStepRate;
	};

	struct GPUBufferDesc
	{
		U32 mByteWidth;
		USAGE mUsage;
		U32 mBindFlags;
		U32 mCPUAccessFlags;
		U32 mMiscFlags;
		// needed for typed and structured buffer types!
		U32 mStructureByteStride; 
		// only needed for typed buffer!
		FORMAT mFormat;		

		GPUBufferDesc() :
			mByteWidth(0),
			mUsage(USAGE_DEFAULT),
			mBindFlags(0),
			mCPUAccessFlags(0),
			mMiscFlags(0),
			mStructureByteStride(0),
			mFormat(FORMAT_UNKNOWN)
		{}
	};

	struct SamplerDesc
	{
		FILTER mFilter;
		TEXTURE_ADDRESS_MODE mAddressU;
		TEXTURE_ADDRESS_MODE mAddressV;
		TEXTURE_ADDRESS_MODE mAddressW;
		F32 mMipLODBias;
		U32 mMaxAnisotropy;
		ComparisonFunc mComparisonFunc;
		F32 mBorderColor[4];
		F32 mMinLOD;
		F32 mMaxLOD;

		SamplerDesc() :
			mFilter(FILTER_MIN_MAG_MIP_POINT),
			mAddressU(TEXTURE_ADDRESS_CLAMP),
			mAddressV(TEXTURE_ADDRESS_CLAMP),
			mAddressW(TEXTURE_ADDRESS_CLAMP),
			mMipLODBias(0.0f),
			mMaxAnisotropy(0),
			mComparisonFunc(COMPARISON_NEVER),
			mBorderColor{0.0f, 0.0f, 0.0f, 0.0f},
			mMinLOD(0.0f),
			mMaxLOD(FLT_MAX)
		{}
	};

	struct SampleDesc
	{
		UINT mCount;
		UINT mQuality;

		SampleDesc() :
			mCount(1),
			mQuality(0)
		{}
	};

	struct TextureDesc
	{
		U32 mWidth;
		U32 mHeight;
		U32 mDepth;
		U32 mArraySize;
		U32 mMipLevels;
		FORMAT mFormat;
		SampleDesc mSampleDesc;
		USAGE mUsage;
		U32 mBindFlags;
		U32 mCPUAccessFlags;
		U32 mMiscFlags;

		TextureDesc() :
			mWidth(0),
			mHeight(0),
			mDepth(0),
			mArraySize(1),
			mMipLevels(1),
			mFormat(FORMAT_UNKNOWN),
			mUsage(USAGE_DEFAULT),
			mBindFlags(0),
			mCPUAccessFlags(0),
			mMiscFlags(0)
		{}
	};

	struct SubresourceData
	{
		const void *mSysMem;
		UINT mSysMemPitch;
		UINT mSysMemSlicePitch;

		SubresourceData() :
			mSysMem(nullptr),
			mSysMemPitch(0),
			mSysMemSlicePitch(0)
		{}
	};
}