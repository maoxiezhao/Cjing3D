#pragma once

#include "renderer\renderableCommon.h"

namespace Cjing3D
{

#define	APPEND_ALIGNED_ELEMENT	( 0xffffffff )

	enum GraphicsThread
	{
		GraphicsThread_IMMEDIATE,
		GraphicsThread_COUNT
	};

	enum SHADERSTAGES
	{
		SHADERSTAGES_VS,
		SHADERSTAGES_GS,
		SHADERSTAGES_HS,
		SHADERSTAGES_DS,
		SHADERSTAGES_PS,
		SHADERSTAGES_CS,
		SHADERSTAGES_COUNT
	};

	enum FORMAT
	{
		FORMAT_UNKNOWN,
		FORMAT_R32G32B32A32_TYPELESS,
		FORMAT_R32G32B32A32_FLOAT,
		FORMAT_R32G32B32A32_UINT,
		FORMAT_R32G32B32A32_SINT,
		FORMAT_R32G32B32_TYPELESS,
		FORMAT_R32G32B32_FLOAT,
		FORMAT_R32G32B32_UINT,
		FORMAT_R32G32B32_SINT,
		FORMAT_R16G16B16A16_TYPELESS,
		FORMAT_R16G16B16A16_FLOAT,
		FORMAT_R16G16B16A16_UNORM,
		FORMAT_R16G16B16A16_UINT,
		FORMAT_R16G16B16A16_SNORM,
		FORMAT_R16G16B16A16_SINT,
		FORMAT_R32G32_TYPELESS,
		FORMAT_R32G32_FLOAT,
		FORMAT_R32G32_UINT,
		FORMAT_R32G32_SINT,
		FORMAT_R32G8X24_TYPELESS,
		FORMAT_D32_FLOAT_S8X24_UINT,
		FORMAT_R32_FLOAT_X8X24_TYPELESS,
		FORMAT_X32_TYPELESS_G8X24_UINT,
		FORMAT_R10G10B10A2_TYPELESS,
		FORMAT_R10G10B10A2_UNORM,
		FORMAT_R10G10B10A2_UINT,
		FORMAT_R11G11B10_FLOAT,
		FORMAT_R8G8B8A8_TYPELESS,
		FORMAT_R8G8B8A8_UNORM,
		FORMAT_R8G8B8A8_UNORM_SRGB,
		FORMAT_R8G8B8A8_UINT,
		FORMAT_R8G8B8A8_SNORM,
		FORMAT_R8G8B8A8_SINT,
		FORMAT_R16G16_TYPELESS,
		FORMAT_R16G16_FLOAT,
		FORMAT_R16G16_UNORM,
		FORMAT_R16G16_UINT,
		FORMAT_R16G16_SNORM,
		FORMAT_R16G16_SINT,
		FORMAT_R32_TYPELESS,
		FORMAT_D32_FLOAT,
		FORMAT_R32_FLOAT,
		FORMAT_R32_UINT,
		FORMAT_R32_SINT,
		FORMAT_R24G8_TYPELESS,
		FORMAT_D24_UNORM_S8_UINT,
		FORMAT_R24_UNORM_X8_TYPELESS,
		FORMAT_X24_TYPELESS_G8_UINT,
		FORMAT_R8G8_TYPELESS,
		FORMAT_R8G8_UNORM,
		FORMAT_R8G8_UINT,
		FORMAT_R8G8_SNORM,
		FORMAT_R8G8_SINT,
		FORMAT_R16_TYPELESS,
		FORMAT_R16_FLOAT,
		FORMAT_D16_UNORM,
		FORMAT_R16_UNORM,
		FORMAT_R16_UINT,
		FORMAT_R16_SNORM,
		FORMAT_R16_SINT,
		FORMAT_R8_TYPELESS,
		FORMAT_R8_UNORM,
		FORMAT_R8_UINT,
		FORMAT_R8_SNORM,
		FORMAT_R8_SINT,
		FORMAT_A8_UNORM,
		FORMAT_R1_UNORM,
		FORMAT_R9G9B9E5_SHAREDEXP,
		FORMAT_R8G8_B8G8_UNORM,
		FORMAT_G8R8_G8B8_UNORM,
		FORMAT_BC1_TYPELESS,
		FORMAT_BC1_UNORM,
		FORMAT_BC1_UNORM_SRGB,
		FORMAT_BC2_TYPELESS,
		FORMAT_BC2_UNORM,
		FORMAT_BC2_UNORM_SRGB,
		FORMAT_BC3_TYPELESS,
		FORMAT_BC3_UNORM,
		FORMAT_BC3_UNORM_SRGB,
		FORMAT_BC4_TYPELESS,
		FORMAT_BC4_UNORM,
		FORMAT_BC4_SNORM,
		FORMAT_BC5_TYPELESS,
		FORMAT_BC5_UNORM,
		FORMAT_BC5_SNORM,
		FORMAT_B5G6R5_UNORM,
		FORMAT_B5G5R5A1_UNORM,
		FORMAT_B8G8R8A8_UNORM,
		FORMAT_B8G8R8X8_UNORM,
		FORMAT_R10G10B10_XR_BIAS_A2_UNORM,
		FORMAT_B8G8R8A8_TYPELESS,
		FORMAT_B8G8R8A8_UNORM_SRGB,
		FORMAT_B8G8R8X8_TYPELESS,
		FORMAT_B8G8R8X8_UNORM_SRGB,
		FORMAT_BC6H_TYPELESS,
		FORMAT_BC6H_UF16,
		FORMAT_BC6H_SF16,
		FORMAT_BC7_TYPELESS,
		FORMAT_BC7_UNORM,
		FORMAT_BC7_UNORM_SRGB,
		FORMAT_AYUV,
		FORMAT_Y410,
		FORMAT_Y416,
		FORMAT_NV12,
		FORMAT_P010,
		FORMAT_P016,
		FORMAT_420_OPAQUE,
		FORMAT_YUY2,
		FORMAT_Y210,
		FORMAT_Y216,
		FORMAT_NV11,
		FORMAT_AI44,
		FORMAT_IA44,
		FORMAT_P8,
		FORMAT_A8P8,
		FORMAT_B4G4R4A4_UNORM,
		FORMAT_FORCE_UINT = 0xffffffff,
	};

	enum DepthWriteMask
	{
		DEPTH_WRITE_MASK_ZERO,
		DEPTH_WRITE_MASK_ALL,
	};

	enum ComparisonFunc
	{
		COMPARISON_NEVER,
		COMPARISON_LESS,
		COMPARISON_EQUAL,
		COMPARISON_LESS_EQUAL,
		COMPARISON_GREATER,
		COMPARISON_NOT_EQUAL,
		COMPARISON_GREATER_EQUAL,
		COMPARISON_ALWAYS,
	};

	enum StencilOp
	{
		STENCIL_OP_KEEP,
		STENCIL_OP_ZERO,
		STENCIL_OP_REPLACE,
		STENCIL_OP_INCR_SAT,
		STENCIL_OP_DECR_SAT,
		STENCIL_OP_INVERT,
		STENCIL_OP_INCR,
		STENCIL_OP_DECR,
	};

	enum Blend
	{
		BLEND_ZERO,
		BLEND_ONE,
		BLEND_SRC_COLOR,
		BLEND_INV_SRC_COLOR,
		BLEND_SRC_ALPHA,
		BLEND_INV_SRC_ALPHA,
		BLEND_DEST_ALPHA,
		BLEND_INV_DEST_ALPHA,
		BLEND_DEST_COLOR,
		BLEND_INV_DEST_COLOR,
		BLEND_SRC_ALPHA_SAT,
		BLEND_BLEND_FACTOR,
		BLEND_INV_BLEND_FACTOR,
		BLEND_SRC1_COLOR,
		BLEND_INV_SRC1_COLOR,
		BLEND_SRC1_ALPHA,
		BLEND_INV_SRC1_ALPHA,
	};

	enum ColorWriteEnable
	{
		COLOR_WRITE_DISABLE = 0,
		COLOR_WRITE_ENABLE_RED = 1,
		COLOR_WRITE_ENABLE_GREEN = 2,
		COLOR_WRITE_ENABLE_BLUE = 4,
		COLOR_WRITE_ENABLE_ALPHA = 8,
		COLOR_WRITE_ENABLE_ALL = (((COLOR_WRITE_ENABLE_RED | COLOR_WRITE_ENABLE_GREEN) | COLOR_WRITE_ENABLE_BLUE) | COLOR_WRITE_ENABLE_ALPHA)

	};

	enum BlendOp
	{
		BLEND_OP_ADD,
		BLEND_OP_SUBTRACT,
		BLEND_OP_REV_SUBTRACT,
		BLEND_OP_MIN,
		BLEND_OP_MAX,
	};

	enum FillMode
	{
		FILL_WIREFRAME,
		FILL_SOLID,
	};
	enum CullMode
	{
		CULL_NONE,
		CULL_FRONT,
		CULL_BACK,
	};

	enum InputClassification
	{
		INPUT_PER_VERTEX_DATA,
		INPUT_PER_INSTANCE_DATA
	};

	enum IndexFormat
	{
		INDEX_FORMAT_16BIT,
		INDEX_FORMAT_32BIT,
	};

	enum BufferScope
	{
		BUFFER_SCOPE_VERTEX,
		BUFFER_SCOPE_PIXEL,
		BUFFER_SCOPE_GLOBAL
	};

	enum USAGE
	{
		USAGE_DEFAULT,		// only GPU read/write
		USAGE_IMMUTABLE,    // GPU read
		USAGE_DYNAMIC,      // CPU write, GPU read
		USAGE_STAGING       // CPU read/write, GPU read/write
	};

	enum BIND_FLAG
	{
		BIND_VERTEX_BUFFER = 0x1L,
		BIND_INDEX_BUFFER = 0x2L,
		BIND_CONSTANT_BUFFER = 0x4L,
		BIND_SHADER_RESOURCE = 0x8L,
		BIND_STREAM_OUTPUT = 0x10L,
		BIND_RENDER_TARGET = 0x20L,
		BIND_DEPTH_STENCIL = 0x40L,
		BIND_UNORDERED_ACCESS = 0x80L,
	};

	enum CPU_ACCESS
	{
		CPU_ACCESS_WRITE = 0x10000L,
		CPU_ACCESS_READ = 0x20000L,
	};

	enum RESOURCE_MISC_FLAG
	{
		RESOURCE_MISC_GENERATE_MIPS = 0x1L,
		RESOURCE_MISC_SHARED = 0x2L,
		RESOURCE_MISC_TEXTURECUBE = 0x4L,
		RESOURCE_MISC_DRAWINDIRECT_ARGS = 0x10L,
		RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS = 0x20L,
		RESOURCE_MISC_BUFFER_STRUCTURED = 0x40L,
		RESOURCE_MISC_TILED = 0x40000L,
	};

	enum PRIMITIVE_TOPOLOGY
	{
		UNDEFINED_TOPOLOGY,
		TRIANGLELIST,
		TRIANGLESTRIP,
		POINTLIST,
		LINELIST,
		PATCHLIST_3,
		PATCHLIST_4,
	};

	enum FILTER
	{
		FILTER_MIN_MAG_MIP_POINT,
		FILTER_MIN_MAG_POINT_MIP_LINEAR,
		FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT,
		FILTER_MIN_POINT_MAG_MIP_LINEAR,
		FILTER_MIN_LINEAR_MAG_MIP_POINT,
		FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
		FILTER_MIN_MAG_LINEAR_MIP_POINT,
		FILTER_MIN_MAG_MIP_LINEAR,
		FILTER_ANISOTROPIC,
		FILTER_COMPARISON_MIN_MAG_MIP_POINT,
		FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR,
		FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT,
		FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR,
		FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT,
		FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
		FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,
		FILTER_COMPARISON_MIN_MAG_MIP_LINEAR,
		FILTER_COMPARISON_ANISOTROPIC,
		FILTER_MINIMUM_MIN_MAG_MIP_POINT,
		FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR,
		FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT,
		FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR,
		FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT,
		FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
		FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT,
		FILTER_MINIMUM_MIN_MAG_MIP_LINEAR,
		FILTER_MINIMUM_ANISOTROPIC,
		FILTER_MAXIMUM_MIN_MAG_MIP_POINT,
		FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR,
		FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT,
		FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR,
		FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT,
		FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
		FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT,
		FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR,
		FILTER_MAXIMUM_ANISOTROPIC,
	};

	enum TEXTURE_ADDRESS_MODE
	{
		TEXTURE_ADDRESS_WRAP,
		TEXTURE_ADDRESS_MIRROR,
		TEXTURE_ADDRESS_CLAMP,
		TEXTURE_ADDRESS_BORDER,
		TEXTURE_ADDRESS_MIRROR_ONCE
	};

	enum CLEAR_FLAG
	{
		CLEAR_DEPTH = 0x1L,
		CLEAR_STENCIL = 0x2L,
	};

	enum GPU_QUERY_TYPE
	{
		GPU_QUERY_TYPE_INVALID,		
		GPU_QUERY_TYPE_EVENT,				// has the GPU reached this point?
		GPU_QUERY_TYPE_OCCLUSION,			// how many samples passed depthstencil test?
		GPU_QUERY_TYPE_OCCLUSION_PREDICATE, // are there any samples that passed depthstencil test
		GPU_QUERY_TYPE_TIMESTAMP,			// retrieve time point of gpu execution
		GPU_QUERY_TYPE_TIMESTAMP_DISJOINT,	// timestamp frequency information
	};

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

		DepthStencilStateDesc() :
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
		BlendTargetBlendStateDesc mRenderTarget[8] = {};

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

		static VertexLayoutDesc VertexData(
			const char* semanticName,
			U32 semanticIndex,
			FORMAT format,
			U32 inputSlot)
		{
			return { semanticName, semanticIndex, format, inputSlot, APPEND_ALIGNED_ELEMENT, INPUT_PER_VERTEX_DATA, 0u };
		}

		static VertexLayoutDesc InstanceData(
			const char* semanticName,
			U32 semanticIndex,
			FORMAT format,
			U32 inputSlot)
		{
			return { semanticName, semanticIndex, format, inputSlot, APPEND_ALIGNED_ELEMENT, INPUT_PER_INSTANCE_DATA, 1u };
		}
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
			mBorderColor{ 0.0f, 0.0f, 0.0f, 0.0f },
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

	struct ClearValue
	{
		float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		float depth = 0.0f;
		U32 stencil = 0;
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
		ClearValue mClearValue = {};

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
		const void* mSysMem;
		UINT mSysMemPitch;
		UINT mSysMemSlicePitch;

		SubresourceData() :
			mSysMem(nullptr),
			mSysMemPitch(0),
			mSysMemSlicePitch(0)
		{}
	};

	struct GPUQueryDesc
	{
		GPU_QUERY_TYPE mGPUQueryType = GPU_QUERY_TYPE_INVALID;
	};

	struct GPUQueryResult
	{
		U64 mTimestamp = 0;
		U64 mTimetampFrequency = 0;
	};
}