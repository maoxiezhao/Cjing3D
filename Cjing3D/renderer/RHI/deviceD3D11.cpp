#include "deviceD3D11.h"
#include "renderer\RHI\d3d11\rhiResourceD3D11.h"

namespace Cjing3D {

namespace {

	// 空指针对象，用于设置nullptr时传递为参数
	const void* const nullptrBlob[128] = { nullptr };

	inline DXGI_FORMAT _ConvertFormat(FORMAT format)
	{
		switch (format)
		{
		case FORMAT_UNKNOWN:
			return DXGI_FORMAT_UNKNOWN;
			break;
		case FORMAT_R32G32B32A32_TYPELESS:
			return DXGI_FORMAT_R32G32B32A32_TYPELESS;
			break;
		case FORMAT_R32G32B32A32_FLOAT:
			return DXGI_FORMAT_R32G32B32A32_FLOAT;
			break;
		case FORMAT_R32G32B32A32_UINT:
			return DXGI_FORMAT_R32G32B32A32_UINT;
			break;
		case FORMAT_R32G32B32A32_SINT:
			return DXGI_FORMAT_R32G32B32A32_SINT;
			break;
		case FORMAT_R32G32B32_TYPELESS:
			return DXGI_FORMAT_R32G32B32_TYPELESS;
			break;
		case FORMAT_R32G32B32_FLOAT:
			return DXGI_FORMAT_R32G32B32_FLOAT;
			break;
		case FORMAT_R32G32B32_UINT:
			return DXGI_FORMAT_R32G32B32_UINT;
			break;
		case FORMAT_R32G32B32_SINT:
			return DXGI_FORMAT_R32G32B32_SINT;
			break;
		case FORMAT_R16G16B16A16_TYPELESS:
			return DXGI_FORMAT_R16G16B16A16_TYPELESS;
			break;
		case FORMAT_R16G16B16A16_FLOAT:
			return DXGI_FORMAT_R16G16B16A16_FLOAT;
			break;
		case FORMAT_R16G16B16A16_UNORM:
			return DXGI_FORMAT_R16G16B16A16_UNORM;
			break;
		case FORMAT_R16G16B16A16_UINT:
			return DXGI_FORMAT_R16G16B16A16_UINT;
			break;
		case FORMAT_R16G16B16A16_SNORM:
			return DXGI_FORMAT_R16G16B16A16_SNORM;
			break;
		case FORMAT_R16G16B16A16_SINT:
			return DXGI_FORMAT_R16G16B16A16_SINT;
			break;
		case FORMAT_R32G32_TYPELESS:
			return DXGI_FORMAT_R32G32_TYPELESS;
			break;
		case FORMAT_R32G32_FLOAT:
			return DXGI_FORMAT_R32G32_FLOAT;
			break;
		case FORMAT_R32G32_UINT:
			return DXGI_FORMAT_R32G32_UINT;
			break;
		case FORMAT_R32G32_SINT:
			return DXGI_FORMAT_R32G32_SINT;
			break;
		case FORMAT_R32G8X24_TYPELESS:
			return DXGI_FORMAT_R32G8X24_TYPELESS;
			break;
		case FORMAT_D32_FLOAT_S8X24_UINT:
			return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
			break;
		case FORMAT_R32_FLOAT_X8X24_TYPELESS:
			return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
			break;
		case FORMAT_X32_TYPELESS_G8X24_UINT:
			return DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;
			break;
		case FORMAT_R10G10B10A2_TYPELESS:
			return DXGI_FORMAT_R10G10B10A2_TYPELESS;
			break;
		case FORMAT_R10G10B10A2_UNORM:
			return DXGI_FORMAT_R10G10B10A2_UNORM;
			break;
		case FORMAT_R10G10B10A2_UINT:
			return DXGI_FORMAT_R10G10B10A2_UINT;
			break;
		case FORMAT_R11G11B10_FLOAT:
			return DXGI_FORMAT_R11G11B10_FLOAT;
			break;
		case FORMAT_R8G8B8A8_TYPELESS:
			return DXGI_FORMAT_R8G8B8A8_TYPELESS;
			break;
		case FORMAT_R8G8B8A8_UNORM:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
			break;
		case FORMAT_R8G8B8A8_UNORM_SRGB:
			return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			break;
		case FORMAT_R8G8B8A8_UINT:
			return DXGI_FORMAT_R8G8B8A8_UINT;
			break;
		case FORMAT_R8G8B8A8_SNORM:
			return DXGI_FORMAT_R8G8B8A8_SNORM;
			break;
		case FORMAT_R8G8B8A8_SINT:
			return DXGI_FORMAT_R8G8B8A8_SINT;
			break;
		case FORMAT_R16G16_TYPELESS:
			return DXGI_FORMAT_R16G16_TYPELESS;
			break;
		case FORMAT_R16G16_FLOAT:
			return DXGI_FORMAT_R16G16_FLOAT;
			break;
		case FORMAT_R16G16_UNORM:
			return DXGI_FORMAT_R16G16_UNORM;
			break;
		case FORMAT_R16G16_UINT:
			return DXGI_FORMAT_R16G16_UINT;
			break;
		case FORMAT_R16G16_SNORM:
			return DXGI_FORMAT_R16G16_SNORM;
			break;
		case FORMAT_R16G16_SINT:
			return DXGI_FORMAT_R16G16_SINT;
			break;
		case FORMAT_R32_TYPELESS:
			return DXGI_FORMAT_R32_TYPELESS;
			break;
		case FORMAT_D32_FLOAT:
			return DXGI_FORMAT_D32_FLOAT;
			break;
		case FORMAT_R32_FLOAT:
			return DXGI_FORMAT_R32_FLOAT;
			break;
		case FORMAT_R32_UINT:
			return DXGI_FORMAT_R32_UINT;
			break;
		case FORMAT_R32_SINT:
			return DXGI_FORMAT_R32_SINT;
			break;
		case FORMAT_R24G8_TYPELESS:
			return DXGI_FORMAT_R24G8_TYPELESS;
			break;
		case FORMAT_D24_UNORM_S8_UINT:
			return DXGI_FORMAT_D24_UNORM_S8_UINT;
			break;
		case FORMAT_R24_UNORM_X8_TYPELESS:
			return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			break;
		case FORMAT_X24_TYPELESS_G8_UINT:
			return DXGI_FORMAT_X24_TYPELESS_G8_UINT;
			break;
		case FORMAT_R8G8_TYPELESS:
			return DXGI_FORMAT_R8G8_TYPELESS;
			break;
		case FORMAT_R8G8_UNORM:
			return DXGI_FORMAT_R8G8_UNORM;
			break;
		case FORMAT_R8G8_UINT:
			return DXGI_FORMAT_R8G8_UINT;
			break;
		case FORMAT_R8G8_SNORM:
			return DXGI_FORMAT_R8G8_SNORM;
			break;
		case FORMAT_R8G8_SINT:
			return DXGI_FORMAT_R8G8_SINT;
			break;
		case FORMAT_R16_TYPELESS:
			return DXGI_FORMAT_R16_TYPELESS;
			break;
		case FORMAT_R16_FLOAT:
			return DXGI_FORMAT_R16_FLOAT;
			break;
		case FORMAT_D16_UNORM:
			return DXGI_FORMAT_D16_UNORM;
			break;
		case FORMAT_R16_UNORM:
			return DXGI_FORMAT_R16_UNORM;
			break;
		case FORMAT_R16_UINT:
			return DXGI_FORMAT_R16_UINT;
			break;
		case FORMAT_R16_SNORM:
			return DXGI_FORMAT_R16_SNORM;
			break;
		case FORMAT_R16_SINT:
			return DXGI_FORMAT_R16_SINT;
			break;
		case FORMAT_R8_TYPELESS:
			return DXGI_FORMAT_R8_TYPELESS;
			break;
		case FORMAT_R8_UNORM:
			return DXGI_FORMAT_R8_UNORM;
			break;
		case FORMAT_R8_UINT:
			return DXGI_FORMAT_R8_UINT;
			break;
		case FORMAT_R8_SNORM:
			return DXGI_FORMAT_R8_SNORM;
			break;
		case FORMAT_R8_SINT:
			return DXGI_FORMAT_R8_SINT;
			break;
		case FORMAT_A8_UNORM:
			return DXGI_FORMAT_A8_UNORM;
			break;
		case FORMAT_R1_UNORM:
			return DXGI_FORMAT_R1_UNORM;
			break;
		case FORMAT_R9G9B9E5_SHAREDEXP:
			return DXGI_FORMAT_R9G9B9E5_SHAREDEXP;
			break;
		case FORMAT_R8G8_B8G8_UNORM:
			return DXGI_FORMAT_R8G8_B8G8_UNORM;
			break;
		case FORMAT_G8R8_G8B8_UNORM:
			return DXGI_FORMAT_G8R8_G8B8_UNORM;
			break;
		case FORMAT_BC1_TYPELESS:
			return DXGI_FORMAT_BC1_TYPELESS;
			break;
		case FORMAT_BC1_UNORM:
			return DXGI_FORMAT_BC1_UNORM;
			break;
		case FORMAT_BC1_UNORM_SRGB:
			return DXGI_FORMAT_BC1_UNORM_SRGB;
			break;
		case FORMAT_BC2_TYPELESS:
			return DXGI_FORMAT_BC2_TYPELESS;
			break;
		case FORMAT_BC2_UNORM:
			return DXGI_FORMAT_BC2_UNORM;
			break;
		case FORMAT_BC2_UNORM_SRGB:
			return DXGI_FORMAT_BC2_UNORM_SRGB;
			break;
		case FORMAT_BC3_TYPELESS:
			return DXGI_FORMAT_BC3_TYPELESS;
			break;
		case FORMAT_BC3_UNORM:
			return DXGI_FORMAT_BC3_UNORM;
			break;
		case FORMAT_BC3_UNORM_SRGB:
			return DXGI_FORMAT_BC3_UNORM_SRGB;
			break;
		case FORMAT_BC4_TYPELESS:
			return DXGI_FORMAT_BC4_TYPELESS;
			break;
		case FORMAT_BC4_UNORM:
			return DXGI_FORMAT_BC4_UNORM;
			break;
		case FORMAT_BC4_SNORM:
			return DXGI_FORMAT_BC4_SNORM;
			break;
		case FORMAT_BC5_TYPELESS:
			return DXGI_FORMAT_BC5_TYPELESS;
			break;
		case FORMAT_BC5_UNORM:
			return DXGI_FORMAT_BC5_UNORM;
			break;
		case FORMAT_BC5_SNORM:
			return DXGI_FORMAT_BC5_SNORM;
			break;
		case FORMAT_B5G6R5_UNORM:
			return DXGI_FORMAT_B5G6R5_UNORM;
			break;
		case FORMAT_B5G5R5A1_UNORM:
			return DXGI_FORMAT_B5G5R5A1_UNORM;
			break;
		case FORMAT_B8G8R8A8_UNORM:
			return DXGI_FORMAT_B8G8R8A8_UNORM;
			break;
		case FORMAT_B8G8R8X8_UNORM:
			return DXGI_FORMAT_B8G8R8X8_UNORM;
			break;
		case FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
			return DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;
			break;
		case FORMAT_B8G8R8A8_TYPELESS:
			return DXGI_FORMAT_B8G8R8A8_TYPELESS;
			break;
		case FORMAT_B8G8R8A8_UNORM_SRGB:
			return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
			break;
		case FORMAT_B8G8R8X8_TYPELESS:
			return DXGI_FORMAT_B8G8R8X8_TYPELESS;
			break;
		case FORMAT_B8G8R8X8_UNORM_SRGB:
			return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
			break;
		case FORMAT_BC6H_TYPELESS:
			return DXGI_FORMAT_BC6H_TYPELESS;
			break;
		case FORMAT_BC6H_UF16:
			return DXGI_FORMAT_BC6H_UF16;
			break;
		case FORMAT_BC6H_SF16:
			return DXGI_FORMAT_BC6H_SF16;
			break;
		case FORMAT_BC7_TYPELESS:
			return DXGI_FORMAT_BC7_TYPELESS;
			break;
		case FORMAT_BC7_UNORM:
			return DXGI_FORMAT_BC7_UNORM;
			break;
		case FORMAT_BC7_UNORM_SRGB:
			return DXGI_FORMAT_BC7_UNORM_SRGB;
			break;
		case FORMAT_AYUV:
			return DXGI_FORMAT_AYUV;
			break;
		case FORMAT_Y410:
			return DXGI_FORMAT_Y410;
			break;
		case FORMAT_Y416:
			return DXGI_FORMAT_Y416;
			break;
		case FORMAT_NV12:
			return DXGI_FORMAT_NV12;
			break;
		case FORMAT_P010:
			return DXGI_FORMAT_P010;
			break;
		case FORMAT_P016:
			return DXGI_FORMAT_P016;
			break;
		case FORMAT_420_OPAQUE:
			return DXGI_FORMAT_420_OPAQUE;
			break;
		case FORMAT_YUY2:
			return DXGI_FORMAT_YUY2;
			break;
		case FORMAT_Y210:
			return DXGI_FORMAT_Y210;
			break;
		case FORMAT_Y216:
			return DXGI_FORMAT_Y216;
			break;
		case FORMAT_NV11:
			return DXGI_FORMAT_NV11;
			break;
		case FORMAT_AI44:
			return DXGI_FORMAT_AI44;
			break;
		case FORMAT_IA44:
			return DXGI_FORMAT_IA44;
			break;
		case FORMAT_P8:
			return DXGI_FORMAT_P8;
			break;
		case FORMAT_A8P8:
			return DXGI_FORMAT_A8P8;
			break;
		case FORMAT_B4G4R4A4_UNORM:
			return DXGI_FORMAT_B4G4R4A4_UNORM;
			break;
		case FORMAT_FORCE_UINT:
			return DXGI_FORMAT_FORCE_UINT;
			break;
		default:
			break;
		}
		return DXGI_FORMAT_UNKNOWN;
	}

	inline D3D11_DEPTH_WRITE_MASK _ConvertDepthWriteMask(DepthWriteMask mask)
	{
		switch (mask)
		{
		case Cjing3D::DEPTH_WRITE_MASK_ZERO:
			return D3D11_DEPTH_WRITE_MASK_ZERO;
		case Cjing3D::DEPTH_WRITE_MASK_ALL:
			return D3D11_DEPTH_WRITE_MASK_ALL;
		default:
			break;
		}
		return D3D11_DEPTH_WRITE_MASK_ZERO;
	}

	inline D3D11_COMPARISON_FUNC _ConvertComparisonFunc(ComparisonFunc func)
	{
		switch (func)
		{
		case Cjing3D::COMPARISON_NEVER:
			return D3D11_COMPARISON_NEVER;
		case Cjing3D::COMPARISON_LESS:
			return D3D11_COMPARISON_LESS;
		case Cjing3D::COMPARISON_EQUAL:
			return D3D11_COMPARISON_EQUAL;
		case Cjing3D::COMPARISON_LESS_EQUAL:
			return D3D11_COMPARISON_LESS_EQUAL;
		case Cjing3D::COMPARISON_GREATER:
			return D3D11_COMPARISON_GREATER;
		case Cjing3D::COMPARISON_NOT_EQUAL:
			return D3D11_COMPARISON_NOT_EQUAL;
		case Cjing3D::COMPARISON_GREATER_EQUAL:
			return D3D11_COMPARISON_GREATER_EQUAL;
		case Cjing3D::COMPARISON_ALWAYS:
			return D3D11_COMPARISON_ALWAYS;
		default:
			break;
		}
		return D3D11_COMPARISON_NEVER;
	}

	inline D3D11_STENCIL_OP _ConvertStencilOp(StencilOp op)
	{
		switch (op)
		{
		case Cjing3D::STENCIL_OP_KEEP:
			return D3D11_STENCIL_OP_KEEP;
		case Cjing3D::STENCIL_OP_ZERO:
			return D3D11_STENCIL_OP_ZERO;
		case Cjing3D::STENCIL_OP_REPLACE:
			return D3D11_STENCIL_OP_REPLACE;
		case Cjing3D::STENCIL_OP_INCR_SAT:
			return D3D11_STENCIL_OP_INCR_SAT;
		case Cjing3D::STENCIL_OP_DECR_SAT:
			return D3D11_STENCIL_OP_DECR_SAT;
		case Cjing3D::STENCIL_OP_INVERT:
			return D3D11_STENCIL_OP_INVERT;
		case Cjing3D::STENCIL_OP_INCR:
			return D3D11_STENCIL_OP_INCR;
		case Cjing3D::STENCIL_OP_DECR:
			return D3D11_STENCIL_OP_DECR;
		default:
			break;
		}
		return D3D11_STENCIL_OP_KEEP;
	}

	inline D3D11_BLEND _ConvertBlend(Blend blend)
	{
		switch (blend)
		{
		case Cjing3D::BLEND_ZERO:
			return D3D11_BLEND_ZERO;
		case Cjing3D::BLEND_ONE:
			return D3D11_BLEND_ONE;
		case Cjing3D::BLEND_SRC_COLOR:
			return D3D11_BLEND_SRC_COLOR;
		case Cjing3D::BLEND_INV_SRC_COLOR:
			return D3D11_BLEND_INV_SRC1_COLOR;
		case Cjing3D::BLEND_SRC_ALPHA:
			return D3D11_BLEND_SRC_ALPHA;
		case Cjing3D::BLEND_INV_SRC_ALPHA:
			return D3D11_BLEND_INV_SRC_ALPHA;
		case Cjing3D::BLEND_DEST_ALPHA:
			return D3D11_BLEND_DEST_ALPHA;
		case Cjing3D::BLEND_INV_DEST_ALPHA:
			return D3D11_BLEND_INV_DEST_ALPHA;
		case Cjing3D::BLEND_DEST_COLOR:
			return D3D11_BLEND_DEST_COLOR;
		case Cjing3D::BLEND_INV_DEST_COLOR:
			return D3D11_BLEND_INV_DEST_COLOR;
		case Cjing3D::BLEND_SRC_ALPHA_SAT:
			return D3D11_BLEND_SRC_ALPHA_SAT;
		case Cjing3D::BLEND_BLEND_FACTOR:
			return D3D11_BLEND_BLEND_FACTOR;
		case Cjing3D::BLEND_INV_BLEND_FACTOR:
			return D3D11_BLEND_INV_BLEND_FACTOR;
		case Cjing3D::BLEND_SRC1_COLOR:
			return D3D11_BLEND_SRC1_COLOR;
		case Cjing3D::BLEND_INV_SRC1_COLOR:
			return D3D11_BLEND_INV_SRC1_COLOR;
		case Cjing3D::BLEND_SRC1_ALPHA:
			return D3D11_BLEND_SRC1_ALPHA;
		case Cjing3D::BLEND_INV_SRC1_ALPHA:
			return D3D11_BLEND_INV_SRC1_ALPHA;
		default:
			break;
		}
		return D3D11_BLEND_ZERO;
	}

	inline D3D11_BLEND_OP _ConvertBlendOp(BlendOp op)
	{
		switch (op)
		{
		case Cjing3D::BLEND_OP_ADD:
			return D3D11_BLEND_OP_ADD;
		case Cjing3D::BLEND_OP_SUBTRACT:
			return D3D11_BLEND_OP_SUBTRACT;
		case Cjing3D::BLEND_OP_REV_SUBTRACT:
			return D3D11_BLEND_OP_REV_SUBTRACT;
		case Cjing3D::BLEND_OP_MIN:
			return D3D11_BLEND_OP_MIN;
		case Cjing3D::BLEND_OP_MAX:
			return D3D11_BLEND_OP_MAX;
		default:
			break;
		}
		return D3D11_BLEND_OP_ADD;
	}

	inline U32 _ParseColorWriteMask(U32 value)
	{
		U32 flag = 0;
		if (value == ColorWriteEnable::COLOR_WRITE_ENABLE_ALL) {
			return D3D11_COLOR_WRITE_ENABLE_ALL;
		}

		if (value & ColorWriteEnable::COLOR_WRITE_ENABLE_RED) {
			flag |= D3D11_COLOR_WRITE_ENABLE_RED;
		}
		if (value & ColorWriteEnable::COLOR_WRITE_ENABLE_GREEN) {
			flag |= D3D11_COLOR_WRITE_ENABLE_GREEN;
		}
		if (value & ColorWriteEnable::COLOR_WRITE_ENABLE_BLUE) {
			flag |= D3D11_COLOR_WRITE_ENABLE_BLUE;
		}
		if (value & ColorWriteEnable::COLOR_WRITE_ENABLE_ALPHA) {
			flag |= D3D11_COLOR_WRITE_ENABLE_ALPHA;
		}
		return flag;
	}

	inline D3D11_FILL_MODE _ConvertFillMode(FillMode mode)
	{
		switch (mode)
		{
		case Cjing3D::FILL_WIREFRAME:
			return D3D11_FILL_WIREFRAME;
		case Cjing3D::FILL_SOLID:
			return D3D11_FILL_SOLID;
		default:
			break;
		}
		return D3D11_FILL_SOLID;
	}

	inline D3D11_CULL_MODE _ConvertCullMode(CullMode mode)
	{
		switch (mode)
		{
		case Cjing3D::CULL_NONE:
			return D3D11_CULL_NONE;
		case Cjing3D::CULL_FRONT:
			return D3D11_CULL_FRONT;
		case Cjing3D::CULL_BACK:
			return D3D11_CULL_BACK;
		default:
			break;
		}
		return D3D11_CULL_NONE;
	}

	inline D3D11_INPUT_CLASSIFICATION _ConvertInputClassification(InputClassification inputClassification)
	{
		switch (inputClassification)
		{
		case Cjing3D::INPUT_PER_VERTEX_DATA:
			return D3D11_INPUT_PER_VERTEX_DATA;
		case Cjing3D::INPUT_PER_INSTANCE_DATA:
			return D3D11_INPUT_PER_INSTANCE_DATA;
		default:
			break;
		}
		return D3D11_INPUT_PER_VERTEX_DATA;
	}

	inline D3D11_USAGE _ConvertUsage(USAGE usage)
	{
		switch (usage)
		{
		case Cjing3D::USAGE_DEFAULT:
			return D3D11_USAGE_DEFAULT;
		case Cjing3D::USAGE_IMMUTABLE:
			return D3D11_USAGE_IMMUTABLE;
		case Cjing3D::USAGE_DYNAMIC:
			return D3D11_USAGE_DYNAMIC;
		case Cjing3D::USAGE_STAGING:
			return D3D11_USAGE_STAGING;
		default:
			break;
		}
		return D3D11_USAGE_DEFAULT;
	}

	inline U32 _ParseBindFlags(U32 value)
	{
		U32 flags = 0;
		if (value & BIND_VERTEX_BUFFER)
			flags |= D3D11_BIND_VERTEX_BUFFER;
		if (value & BIND_INDEX_BUFFER)
			flags |= D3D11_BIND_INDEX_BUFFER;
		if (value & BIND_CONSTANT_BUFFER)
			flags |= D3D11_BIND_CONSTANT_BUFFER;
		if (value & BIND_SHADER_RESOURCE)
			flags |= D3D11_BIND_SHADER_RESOURCE;
		if (value & BIND_STREAM_OUTPUT)
			flags |= D3D11_BIND_STREAM_OUTPUT;
		if (value & BIND_RENDER_TARGET)
			flags |= D3D11_BIND_RENDER_TARGET;
		if (value & BIND_DEPTH_STENCIL)
			flags |= D3D11_BIND_DEPTH_STENCIL;
		if (value & BIND_UNORDERED_ACCESS)
			flags |= D3D11_BIND_UNORDERED_ACCESS;

		return flags;
	}

	inline U32 _ParseCPUAccessFlags(U32 value)
	{
		U32 flags = 0;
		if (value & CPU_ACCESS_WRITE)
			flags |= D3D11_CPU_ACCESS_WRITE;
		if (value & CPU_ACCESS_READ)
			flags |= D3D11_CPU_ACCESS_READ;

		return flags;
	}

	inline UINT _ParseResourceMiscFlags(UINT value)
	{
		UINT flags = 0;
		if (value & RESOURCE_MISC_SHARED)
			flags |= D3D11_RESOURCE_MISC_SHARED;
		if (value & RESOURCE_MISC_TEXTURECUBE)
			flags |= D3D11_RESOURCE_MISC_TEXTURECUBE;
		if (value & RESOURCE_MISC_DRAWINDIRECT_ARGS)
			flags |= D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
		if (value & RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
			flags |= D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		if (value & RESOURCE_MISC_BUFFER_STRUCTURED)
			flags |= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		if (value & RESOURCE_MISC_TILED)
			flags |= D3D11_RESOURCE_MISC_TILED;

		return flags;
	}

	inline D3D11_FILTER _ConvertFilter(FILTER filter)
	{
		switch (filter)
		{
		case FILTER_MIN_MAG_MIP_POINT:
			return D3D11_FILTER_MIN_MAG_MIP_POINT;
			break;
		case FILTER_MIN_MAG_POINT_MIP_LINEAR:
			return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
			break;
		case FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT:
			return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
			break;
		case FILTER_MIN_POINT_MAG_MIP_LINEAR:
			return D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
			break;
		case FILTER_MIN_LINEAR_MAG_MIP_POINT:
			return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
			break;
		case FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
			return D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
			break;
		case FILTER_MIN_MAG_LINEAR_MIP_POINT:
			return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
			break;
		case FILTER_MIN_MAG_MIP_LINEAR:
			return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			break;
		case FILTER_ANISOTROPIC:
			return D3D11_FILTER_ANISOTROPIC;
			break;
		case FILTER_COMPARISON_MIN_MAG_MIP_POINT:
			return D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
			break;
		case FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR:
			return D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
			break;
		case FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT:
			return D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT;
			break;
		case FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR:
			return D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR;
			break;
		case FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT:
			return D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
			break;
		case FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
			return D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
			break;
		case FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT:
			return D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
			break;
		case FILTER_COMPARISON_MIN_MAG_MIP_LINEAR:
			return D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
			break;
		case FILTER_COMPARISON_ANISOTROPIC:
			return D3D11_FILTER_COMPARISON_ANISOTROPIC;
			break;
		case FILTER_MINIMUM_MIN_MAG_MIP_POINT:
			return D3D11_FILTER_MINIMUM_MIN_MAG_MIP_POINT;
			break;
		case FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR:
			return D3D11_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR;
			break;
		case FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT:
			return D3D11_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT;
			break;
		case FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR:
			return D3D11_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR;
			break;
		case FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT:
			return D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT;
			break;
		case FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
			return D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
			break;
		case FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT:
			return D3D11_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT;
			break;
		case FILTER_MINIMUM_MIN_MAG_MIP_LINEAR:
			return D3D11_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR;
			break;
		case FILTER_MINIMUM_ANISOTROPIC:
			return D3D11_FILTER_MINIMUM_ANISOTROPIC;
			break;
		case FILTER_MAXIMUM_MIN_MAG_MIP_POINT:
			return D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_POINT;
			break;
		case FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR:
			return D3D11_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR;
			break;
		case FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT:
			return D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT;
			break;
		case FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR:
			return D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR;
			break;
		case FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT:
			return D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT;
			break;
		case FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
			return D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
			break;
		case FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT:
			return D3D11_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT;
			break;
		case FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR:
			return D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR;
			break;
		case FILTER_MAXIMUM_ANISOTROPIC:
			return D3D11_FILTER_MAXIMUM_ANISOTROPIC;
			break;
		default:
			break;
		}
		return D3D11_FILTER_MIN_MAG_MIP_POINT;
	}

	inline D3D11_TEXTURE_ADDRESS_MODE _ConvertTextureAddressMode(TEXTURE_ADDRESS_MODE mode)
	{
		switch (mode)
		{
		case Cjing3D::TEXTURE_ADDRESS_WRAP:
			return D3D11_TEXTURE_ADDRESS_WRAP;
		case Cjing3D::TEXTURE_ADDRESS_MIRROR:
			return D3D11_TEXTURE_ADDRESS_MIRROR;
		case Cjing3D::TEXTURE_ADDRESS_CLAMP:
			return D3D11_TEXTURE_ADDRESS_CLAMP;
		case Cjing3D::TEXTURE_ADDRESS_BORDER:
			return D3D11_TEXTURE_ADDRESS_BORDER;
		case Cjing3D::TEXTURE_ADDRESS_MIRROR_ONCE:
			return D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
		default:
			break;
		}
		return D3D11_TEXTURE_ADDRESS_WRAP;
	}

	inline D3D11_TEXTURE2D_DESC _ConvertTexture2DDesc(const TextureDesc* desc)
	{
		D3D11_TEXTURE2D_DESC texture2DDesc = {};
		texture2DDesc.Width = desc->mWidth;
		texture2DDesc.Height = desc->mHeight;
		texture2DDesc.MipLevels = desc->mMipLevels;
		texture2DDesc.ArraySize = desc->mArraySize;
		texture2DDesc.Format = _ConvertFormat(desc->mFormat);
		texture2DDesc.SampleDesc.Count = desc->mSampleDesc.mCount;
		texture2DDesc.SampleDesc.Quality = desc->mSampleDesc.mQuality;
		texture2DDesc.Usage = _ConvertUsage(desc->mUsage);
		texture2DDesc.BindFlags = _ParseBindFlags(desc->mBindFlags);
		texture2DDesc.CPUAccessFlags = _ParseCPUAccessFlags(desc->mCPUAccessFlags);
		texture2DDesc.MiscFlags = _ParseResourceMiscFlags(desc->mMiscFlags);

		return texture2DDesc;
	}

	inline D3D11_SUBRESOURCE_DATA _ConvertSubresourceData(const SubresourceData& pInitialData)
	{
		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = pInitialData.mSysMem;
		data.SysMemPitch = pInitialData.mSysMemPitch;
		data.SysMemSlicePitch = pInitialData.mSysMemSlicePitch;

		return data;
	}

	inline UINT _ConvertClearFlag(UINT flag)
	{
		UINT result = 0;
		if (flag & CLEAR_DEPTH)
			result |= D3D11_CLEAR_DEPTH;
		if (flag & CLEAR_STENCIL)
			result |= D3D11_CLEAR_STENCIL;
		return result;
	}

	inline bool IsFormatStencilSupport(FORMAT value)
	{
		switch (value)
		{
		case FORMAT_R32G8X24_TYPELESS:
		case FORMAT_D32_FLOAT_S8X24_UINT:
		case FORMAT_R24G8_TYPELESS:
		case FORMAT_D24_UNORM_S8_UINT:
			return true;
		}

		return false;
	}

}

GraphicsDeviceD3D11::GraphicsDeviceD3D11(HWND window, bool fullScreen, bool debugLayer):
	GraphicsDevice(GraphicsDeviceType_directx11),
	mWindow(window),
	mDebugLayer(debugLayer),
	mDevice(),
	mDeviceContext(),
	mSwapChain()
{
	mIsFullScreen = fullScreen;

	mBackBufferFormat = FORMAT_R8G8B8A8_UNORM;

	RECT rect = RECT();
	GetClientRect(window, &rect);
	mScreenSize[0] = rect.right - rect.left;
	mScreenSize[1] = rect.bottom - rect.top;

	ClearPrevStates();
}

void GraphicsDeviceD3D11::Initialize()
{
	GraphicsDevice::Initialize();

	// 初始化device
	InitializeDevice();

	// 初始化swapchain
	mSwapChain = std::make_unique<SwapChainD3D11>(
		*mDevice.Get(),
		GetDeviceContext(GraphicsThread_IMMEDIATE),
		mWindow,
		mScreenSize,
		_ConvertFormat(GetBackBufferFormat())
	);

	// 创建ID3DUserDefinedAnnotation for GraphicsThread_IMMEDIATE
	{
		const auto result = GetDeviceContext(GraphicsThread_IMMEDIATE).QueryInterface(
			__uuidof(mUserDefinedAnnotations[GraphicsThread_IMMEDIATE]),
			reinterpret_cast<void**>(&mUserDefinedAnnotations[GraphicsThread_IMMEDIATE]));
		Debug::ThrowIfFailed(result, "Failed to create ID3DUserDefinedAnnotation: %08X", result);
	}

	// 检查是否支持多线程
	D3D11_FEATURE_DATA_THREADING threadingFeature;
	mDevice->CheckFeatureSupport(D3D11_FEATURE_THREADING, &threadingFeature, sizeof(threadingFeature));
	if (threadingFeature.DriverConcurrentCreates && threadingFeature.DriverCommandLists)
	{
		mIsMultithreadedRendering = true;
		
		for (int i = 0; i < GraphicsThread_COUNT; i++)
		{
			if (i == static_cast<U32>(GraphicsThread_IMMEDIATE)) {
				continue;
			}
			// 创建deferredContext
			ComPtr<ID3D11DeviceContext> deviceContext;
			{
				const auto result = mDevice->CreateDeferredContext(0, &deviceContext);
				Debug::ThrowIfFailed(result, "Failed to create deferred context: %08X", result);
			}

			// 创建ID3DUserDefinedAnnotation
			{
				const auto result = deviceContext->QueryInterface(
					__uuidof(mUserDefinedAnnotations[i]),
					reinterpret_cast<void**>(&mUserDefinedAnnotations[i]));
				Debug::ThrowIfFailed(result, "Failed to create ID3DUserDefinedAnnotation: %08X", result);
			}
			deviceContext.As(&mDeviceContext[i]);
		}
	}

	// 创建视口
	mViewport.mWidth = static_cast<F32>(mScreenSize[0]);
	mViewport.mHeight = static_cast<F32>(mScreenSize[1]);
	mViewport.mTopLeftX = 0.0f;
	mViewport.mTopLeftY = 0.0f;
	mViewport.mMinDepth = 0.0f;
	mViewport.mMaxDepth = 1.0f;

	// 初始化gpu allocator
	mGPUAllocatorDesc.mByteWidth = 4 * 1024 * 1024;
	mGPUAllocatorDesc.mBindFlags = BIND_SHADER_RESOURCE | BIND_INDEX_BUFFER | BIND_VERTEX_BUFFER;
	mGPUAllocatorDesc.mUsage = USAGE_DYNAMIC;
	mGPUAllocatorDesc.mCPUAccessFlags = CPU_ACCESS_WRITE;
	mGPUAllocatorDesc.mMiscFlags = RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
	CreateBuffer(&mGPUAllocatorDesc, mGPUAllocator.buffer, nullptr);
}

void GraphicsDeviceD3D11::Uninitialize()
{
	ClearPrevStates();

	GraphicsDevice::Uninitialize();

	mSwapChain.reset();

	for (int i = 0; i < GraphicsThread_COUNT; i++) {
		mDeviceContext[i]->ClearState();
		mDeviceContext[i].Reset();
	}

	// debug, report live objects
	if (mDebugLayer == true)
	{
#ifdef DEBUG_REPORT_LIVE_DEVICE_OBJECTS
		ComPtr<ID3D11Debug> d3dDebug = nullptr;
		HRESULT hr = mDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(d3dDebug.ReleaseAndGetAddressOf()));
		if (SUCCEEDED(hr)) {
			d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		}
#endif
	}

	if (mDevice != nullptr) {
		mDevice.Reset();
	}
}

void GraphicsDeviceD3D11::PresentBegin()
{
	BindViewports(&mViewport, 1, GraphicsThread_IMMEDIATE);

	ID3D11RenderTargetView* renderTargetView = &mSwapChain->GetRenderTargetView();
	float clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
	auto& deviceContext = GetDeviceContext(GraphicsThread_IMMEDIATE);
	deviceContext.OMSetRenderTargets(1, &renderTargetView, 0);
	deviceContext.ClearRenderTargetView(renderTargetView, clearColor);
}

void GraphicsDeviceD3D11::PresentEnd()
{
	GraphicsDevice::PresentEnd();

	mSwapChain->Present(mIsVsync);

	auto& deviceContext = GetDeviceContext(GraphicsThread_IMMEDIATE);
	deviceContext.OMSetRenderTargets(0, nullptr, nullptr);
	deviceContext.ClearState();

	BindPipelineState(PipelineState());
	ClearPrevStates();

	mCurrentFrameCount++;
}

void GraphicsDeviceD3D11::BeginEvent(const std::string & name)
{
	GetUserDefineAnnotation(GraphicsThread_IMMEDIATE).BeginEvent(
		std::wstring(name.begin(), name.end()).c_str()
	);
}

void GraphicsDeviceD3D11::EndEvent()
{
	GetUserDefineAnnotation(GraphicsThread_IMMEDIATE).EndEvent();
}

// 绑定视口
void GraphicsDeviceD3D11::BindViewports(const ViewPort * viewports, U32 numViewports, GraphicsThread threadID)
{
	Debug::ThrowIfFailed(numViewports <= 6);

	D3D11_VIEWPORT d3dViewports[6];
	for (int i = 0; i < numViewports; i++)
	{
		d3dViewports[i].Width = viewports[i].mWidth;
		d3dViewports[i].Height = viewports[i].mHeight;
		d3dViewports[i].MinDepth = viewports[i].mMinDepth;
		d3dViewports[i].MaxDepth = viewports[i].mMaxDepth;
		d3dViewports[i].TopLeftX = viewports[i].mTopLeftX;
		d3dViewports[i].TopLeftY = viewports[i].mTopLeftY;
	}

	GetDeviceContext(threadID).RSSetViewports(numViewports, d3dViewports);
}

HRESULT GraphicsDeviceD3D11::CreateDepthStencilState(const DepthStencilStateDesc & desc, DepthStencilState & state)
{
	state.SetDesc(desc);

	D3D11_DEPTH_STENCIL_DESC depthDesc = {};
	depthDesc.DepthEnable		= desc.mDepthEnable;
	depthDesc.DepthWriteMask	= _ConvertDepthWriteMask(desc.mDepthWriteMask);
	depthDesc.DepthFunc			= _ConvertComparisonFunc(desc.mDepthFunc);
	depthDesc.StencilEnable		= desc.mStencilEnable;
	depthDesc.StencilReadMask	= desc.mStencilReadMask;
	depthDesc.StencilWriteMask	= desc.mStencilWriteMask;

	depthDesc.FrontFace.StencilFunc			 = _ConvertComparisonFunc(desc.mFrontFace.mStencilFunc);
	depthDesc.FrontFace.StencilPassOp		 = _ConvertStencilOp(desc.mFrontFace.mStencilPassOp);
	depthDesc.FrontFace.StencilFailOp		 = _ConvertStencilOp(desc.mFrontFace.mStencilFailOp);
	depthDesc.FrontFace.StencilDepthFailOp   = _ConvertStencilOp(desc.mFrontFace.mStencilDepthFailOp);

	depthDesc.BackFace.StencilFunc			 = _ConvertComparisonFunc(desc.mBackFace.mStencilFunc);
	depthDesc.BackFace.StencilPassOp		 = _ConvertStencilOp(desc.mBackFace.mStencilPassOp);
	depthDesc.BackFace.StencilFailOp		 = _ConvertStencilOp(desc.mBackFace.mStencilFailOp);
	depthDesc.BackFace.StencilDepthFailOp    = _ConvertStencilOp(desc.mBackFace.mStencilDepthFailOp);

	auto rhiState = RegisterGraphicsDeviceChild<DepthStencilStateD3D11>(state);
	return mDevice->CreateDepthStencilState(&depthDesc, &rhiState->mHandle);
}

HRESULT GraphicsDeviceD3D11::CreateBlendState(const BlendStateDesc & desc, BlendState & state)
{
	state.SetDesc(desc);

	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.AlphaToCoverageEnable = desc.mAlphaToCoverageEnable;
	blendDesc.IndependentBlendEnable = desc.mIndependentBlendEnable;

	for (int i = 0; i < 8; i++)
	{
		blendDesc.RenderTarget[i].BlendEnable = desc.mRenderTarget[i].mBlendEnable;
		blendDesc.RenderTarget[i].SrcBlend = _ConvertBlend(desc.mRenderTarget[i].mSrcBlend);
		blendDesc.RenderTarget[i].DestBlend = _ConvertBlend(desc.mRenderTarget[i].mDstBlend);
		blendDesc.RenderTarget[i].BlendOp = _ConvertBlendOp(desc.mRenderTarget[i].mBlendOp);
		blendDesc.RenderTarget[i].SrcBlendAlpha = _ConvertBlend(desc.mRenderTarget[i].mSrcBlendAlpha);
		blendDesc.RenderTarget[i].DestBlendAlpha = _ConvertBlend(desc.mRenderTarget[i].mDstBlendAlpha);
		blendDesc.RenderTarget[i].BlendOpAlpha = _ConvertBlendOp(desc.mRenderTarget[i].mBlendOpAlpha);

		blendDesc.RenderTarget[i].RenderTargetWriteMask = _ParseColorWriteMask(desc.mRenderTarget[i].mRenderTargetWriteMask);
	}

	auto rhiState = RegisterGraphicsDeviceChild<BlendStateD3D11>(state);
	return mDevice->CreateBlendState(&blendDesc, &rhiState->mHandle);
}

HRESULT GraphicsDeviceD3D11::CreateRasterizerState(const RasterizerStateDesc & desc, RasterizerState & state)
{
	state.SetDesc(desc);

	D3D11_RASTERIZER_DESC rasterizerDesc = {};

	rasterizerDesc.FillMode = _ConvertFillMode(desc.mFillMode);
	rasterizerDesc.CullMode = _ConvertCullMode(desc.mCullMode);
	rasterizerDesc.FrontCounterClockwise = desc.mFrontCounterClockwise;

	// Bias = (float)DepthBias * r + SlopeScaledDepthBias * MaxDepthSlope;
	rasterizerDesc.DepthBias = desc.mDepthBias;
	rasterizerDesc.DepthBiasClamp = desc.mDepthBiasClamp;
	rasterizerDesc.SlopeScaledDepthBias = desc.mSlopeScaleDepthBias;

	rasterizerDesc.DepthClipEnable = desc.mDepthClipEnable;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.MultisampleEnable = desc.mMultisampleEnable;
	rasterizerDesc.AntialiasedLineEnable = desc.mAntialiaseLineEnable;

	auto rhiState = RegisterGraphicsDeviceChild<RasterizerStateD3D11>(state);
	return mDevice->CreateRasterizerState(&rasterizerDesc, &rhiState->mHandle);
}

// 创建输入布局
HRESULT GraphicsDeviceD3D11::CreateInputLayout(VertexLayoutDesc* desc, U32 numElements, Shader& shader, InputLayout & inputLayout)
{
	D3D11_INPUT_ELEMENT_DESC* inputLayoutdescs = new D3D11_INPUT_ELEMENT_DESC[numElements];
	for (int i = 0; i < numElements; i++)
	{
		inputLayoutdescs[i].SemanticName	= desc[i].mSemanticName;
		inputLayoutdescs[i].SemanticIndex = desc[i].mSemanticIndex;
		inputLayoutdescs[i].Format = _ConvertFormat(desc[i].mFormat);
		inputLayoutdescs[i].InputSlot  = desc[i].mInputSlot;
		inputLayoutdescs[i].AlignedByteOffset = desc[i].mAlignedByteOffset;
		inputLayoutdescs[i].InputSlotClass = _ConvertInputClassification(desc[i].mInputSlotClass);
		inputLayoutdescs[i].InstanceDataStepRate  = desc[i].mInstanceDataStepRate;
	}

	auto rhiState = RegisterGraphicsDeviceChild<InputLayoutD3D11>(inputLayout);
	return mDevice->CreateInputLayout(inputLayoutdescs, numElements, shader.mByteCode.mByteData, shader.mByteCode.mByteLength, &rhiState->mHandle);
}

HRESULT GraphicsDeviceD3D11::CreateShader(SHADERSTAGES stage, const void* bytecode, size_t length, Shader& shader)
{
	shader.mByteCode.mByteLength = length;
	shader.mByteCode.mByteData = new BYTE[length];
	memcpy(shader.mByteCode.mByteData, bytecode, length);

	shader.mStage = stage;

	HRESULT hr = E_FAIL;
	switch (stage)
	{
	case Cjing3D::SHADERSTAGES_VS:
	{
		auto rhiState = RegisterGraphicsDeviceChild<VertexShaderD3D11>(shader);
		hr = mDevice->CreateVertexShader(bytecode, length, nullptr, &rhiState->mHandle);
	}
	break;
	case Cjing3D::SHADERSTAGES_HS:
	{
		auto rhiState = RegisterGraphicsDeviceChild<HullShaderD3D11>(shader);
		hr = mDevice->CreateHullShader(bytecode, length, nullptr, &rhiState->mHandle);
	}
	break;
	case Cjing3D::SHADERSTAGES_DS:
	{
		auto rhiState = RegisterGraphicsDeviceChild<DomainShaderD3D11>(shader);
		hr = mDevice->CreateDomainShader(bytecode, length, nullptr, &rhiState->mHandle);
	}
	break;
	case Cjing3D::SHADERSTAGES_PS:
	{
		auto rhiState = RegisterGraphicsDeviceChild<PixelShaderD3D11>(shader);
		hr = mDevice->CreatePixelShader(bytecode, length, nullptr, &rhiState->mHandle);
	}
	break;
	case Cjing3D::SHADERSTAGES_CS:
	{
		auto rhiState = RegisterGraphicsDeviceChild<ComputeShaderD3D11>(shader);
		hr = mDevice->CreateComputeShader(bytecode, length, nullptr, &rhiState->mHandle);
	}
	break;
	default:
		break;
	}
	return hr;
}

HRESULT GraphicsDeviceD3D11::CreateBuffer(const GPUBufferDesc * desc, GPUBuffer & buffer, const SubresourceData* initialData)
{
	buffer.SetDesc(*desc);
	buffer.mCurrType = GPU_RESOURCE_TYPE::BUFFER;

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = desc->mByteWidth;
	bufferDesc.Usage = _ConvertUsage(desc->mUsage);
	bufferDesc.BindFlags = _ParseBindFlags(desc->mBindFlags);
	bufferDesc.CPUAccessFlags = _ParseCPUAccessFlags(desc->mCPUAccessFlags);
	bufferDesc.MiscFlags = _ParseResourceMiscFlags(desc->mMiscFlags);
	bufferDesc.StructureByteStride = desc->mStructureByteStride;

	D3D11_SUBRESOURCE_DATA* subresource_data = nullptr;
	if (initialData != nullptr)
	{
		subresource_data = new D3D11_SUBRESOURCE_DATA;
		(*subresource_data) = _ConvertSubresourceData(*initialData);
	}

	auto rhiState = RegisterGraphicsDeviceChild<GPUResourceD3D11>(buffer);
	HRESULT result = mDevice->CreateBuffer(&bufferDesc, subresource_data, (ID3D11Buffer**)rhiState->mResource.ReleaseAndGetAddressOf());
	SAFE_DELETE_ARRAY(subresource_data);

	if (SUCCEEDED(result))
	{
		// 根据miscFlags创建SRV
		if (bufferDesc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			if (bufferDesc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
			{
				// byte raw buffer
				srvDesc.Format = DXGI_FORMAT_R32_TYPELESS; // ????
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
				srvDesc.Buffer.FirstElement = 0;
				srvDesc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
				srvDesc.Buffer.NumElements = desc->mByteWidth / 4;	// 4字节存储
			}
			else if (bufferDesc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
			{
				srvDesc.Format = DXGI_FORMAT_UNKNOWN; // ????
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
				srvDesc.Buffer.FirstElement = 0;
				srvDesc.Buffer.NumElements = desc->mByteWidth / desc->mStructureByteStride;
			}
			else
			{
				// typed buffer，存储为numElements个structureByteStride大小的元素
				srvDesc.Format = _ConvertFormat(desc->mFormat);
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
				srvDesc.Buffer.FirstElement = 0;
				srvDesc.Buffer.NumElements = desc->mByteWidth / desc->mStructureByteStride;
			}

			result = mDevice->CreateShaderResourceView(rhiState->mResource.Get(), &srvDesc, &rhiState->mSRV);
		}
		if (bufferDesc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
		{
			D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
			uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			uavDesc.Buffer.FirstElement = 0;

			if (bufferDesc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
			{
				// byte raw buffer
				uavDesc.Format = DXGI_FORMAT_R32_TYPELESS; // ????
				uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
				uavDesc.Buffer.NumElements = desc->mByteWidth / 4;	// 4字节存储
			}
			else if (bufferDesc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
			{
				uavDesc.Format = DXGI_FORMAT_UNKNOWN; // ????
				uavDesc.Buffer.NumElements = desc->mByteWidth / desc->mStructureByteStride;
			}
			else
			{
				// typed buffer，存储为numElements个structureByteStride大小的元素
				uavDesc.Format = _ConvertFormat(desc->mFormat);
				uavDesc.Buffer.NumElements = desc->mByteWidth / desc->mStructureByteStride;
			}

			result = mDevice->CreateUnorderedAccessView(rhiState->mResource.Get(), &uavDesc, &rhiState->mUAV);
		}
	}

	return result;
}

void GraphicsDeviceD3D11::UpdateBuffer(GPUBuffer & buffer, const void * data, U32 dataSize)
{
	const auto& desc = buffer.GetDesc();
	Debug::CheckAssertion(desc.mUsage != USAGE_IMMUTABLE, "Cannot update IMMUTABLE Buffer");
	Debug::CheckAssertion(desc.mByteWidth >= dataSize, "Data size is unable");

	dataSize = std::min(dataSize, desc.mByteWidth);
	if (dataSize <= 0) {
		return;
	}

	auto rhiState = GetGraphicsDeviceChildState<GPUResourceD3D11>(buffer);
	if (rhiState == nullptr) {
		Debug::Warning("Faild to update null constant buffer");
		return;
	}

	// 仅当usage为USAGE_DYNAMIC时，CPU使用map/unmap写数据
	if (desc.mUsage == USAGE_DYNAMIC)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT result = GetDeviceContext(GraphicsThread_IMMEDIATE).Map(rhiState->mResource.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		Debug::ThrowIfFailed(result, "Failed to map buffer:%08x", result);

		memcpy(mappedResource.pData, data, dataSize);

		GetDeviceContext(GraphicsThread_IMMEDIATE).Unmap(rhiState->mResource.Get(), 0);
	}
	else if (desc.mBindFlags & BIND_CONSTANT_BUFFER)
	{
		GetDeviceContext(GraphicsThread_IMMEDIATE).UpdateSubresource(rhiState->mResource.Get(), 0, nullptr, data, 0, 0);
	}
	else
	{
		D3D11_BOX box = {};
		box.left = 0;
		box.right = dataSize;
		box.top = 0;
		box.bottom = 1;
		box.front = 0;
		box.back = 1;

		GetDeviceContext(GraphicsThread_IMMEDIATE).UpdateSubresource(rhiState->mResource.Get(), 0, &box, data, 0, 0);
	}
}

void GraphicsDeviceD3D11::BindConstantBuffer(SHADERSTAGES stage, GPUBuffer & buffer, U32 slot)
{
	auto rhiState = GetGraphicsDeviceChildState<GPUResourceD3D11>(buffer);
	if (rhiState == nullptr) {
		Debug::Warning("Faild to bind null constant buffer");
		return;
	}

	ID3D11Buffer* d3dBuffer = (ID3D11Buffer*)rhiState->mResource.Get();
	switch (stage)
	{
	case Cjing3D::SHADERSTAGES_VS:
		GetDeviceContext(GraphicsThread_IMMEDIATE).VSSetConstantBuffers(slot, 1, &d3dBuffer);
		break;
	case Cjing3D::SHADERSTAGES_GS:
		GetDeviceContext(GraphicsThread_IMMEDIATE).GSSetConstantBuffers(slot, 1, &d3dBuffer);
		break;
	case Cjing3D::SHADERSTAGES_HS:
		GetDeviceContext(GraphicsThread_IMMEDIATE).HSSetConstantBuffers(slot, 1, &d3dBuffer);
		break;
	case Cjing3D::SHADERSTAGES_DS:
		GetDeviceContext(GraphicsThread_IMMEDIATE).DSSetConstantBuffers(slot, 1, &d3dBuffer);
		break;
	case Cjing3D::SHADERSTAGES_PS:
		GetDeviceContext(GraphicsThread_IMMEDIATE).PSSetConstantBuffers(slot, 1, &d3dBuffer);
		break;
	case Cjing3D::SHADERSTAGES_CS:
		GetDeviceContext(GraphicsThread_IMMEDIATE).CSSetConstantBuffers(slot, 1, &d3dBuffer);
		break;
	default:
		break;
	}
}

void GraphicsDeviceD3D11::BindIndexBuffer(GPUBuffer & buffer, IndexFormat format, U32 offset)
{
	auto rhiState = GetGraphicsDeviceChildState<GPUResourceD3D11>(buffer);
	if (rhiState == nullptr) {
		Debug::Warning("Faild to bind null index buffer");
		return;
	}

	ID3D11Buffer* d3d11Buffer = (ID3D11Buffer*)rhiState->mResource.Get();
	DXGI_FORMAT d3d11Format = (format == IndexFormat::INDEX_FORMAT_32BIT) ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;
	GetDeviceContext(GraphicsThread_IMMEDIATE).IASetIndexBuffer(d3d11Buffer, d3d11Format, offset);
}

void GraphicsDeviceD3D11::BindVertexBuffer(GPUBuffer* const* buffer, U32 slot, U32 num, const U32 * strides, const U32 * offsets)
{
	ID3D11Buffer* buffers[8] = { nullptr };
	for (size_t i = 0; i < num; i++)
	{
		if (buffer[i] == nullptr) {
			continue;
		}

		auto rhiState = GetGraphicsDeviceChildState<GPUResourceD3D11>(*buffer[i]);
		if (rhiState == nullptr) {
			continue;
		}

		buffers[i] = (ID3D11Buffer*)rhiState->mResource.Get();
	}
	GetDeviceContext(GraphicsThread_IMMEDIATE).IASetVertexBuffers(slot, num, buffers, strides, offsets);
}

void GraphicsDeviceD3D11::ClearVertexBuffer()
{
	GPUBuffer* vbs[] = {
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
	};
	U32 strides[] = {
		0,0,0,0,0,0,0,0
	};
	U32 offsets[] = {
		0,0,0,0,0,0,0,0
	};
	BindVertexBuffer(vbs, 0, ARRAYSIZE(vbs), strides, offsets);
}

// 创建采样器状态
HRESULT GraphicsDeviceD3D11::CreateSamplerState(const SamplerDesc * desc, SamplerState & state)
{
	state.SetDesc(*desc);

	D3D11_SAMPLER_DESC samplerDesc = {};

	samplerDesc.Filter = _ConvertFilter(desc->mFilter);
	samplerDesc.AddressU = _ConvertTextureAddressMode(desc->mAddressU);
	samplerDesc.AddressV = _ConvertTextureAddressMode(desc->mAddressV);
	samplerDesc.AddressW = _ConvertTextureAddressMode(desc->mAddressW);
	samplerDesc.MipLODBias = desc->mMipLODBias;
	samplerDesc.MaxAnisotropy = desc->mMaxAnisotropy;
	samplerDesc.ComparisonFunc = _ConvertComparisonFunc(desc->mComparisonFunc);
	samplerDesc.BorderColor[0] = desc->mBorderColor[0];
	samplerDesc.BorderColor[1] = desc->mBorderColor[1];
	samplerDesc.BorderColor[2] = desc->mBorderColor[2];
	samplerDesc.BorderColor[3] = desc->mBorderColor[3];
	samplerDesc.MinLOD = desc->mMinLOD;
	samplerDesc.MaxLOD = desc->mMaxLOD;

	auto rhiState = RegisterGraphicsDeviceChild<SamplerStateD3D11>(state);
	return mDevice->CreateSamplerState(&samplerDesc, &rhiState->mHandle);
}

void GraphicsDeviceD3D11::BindSamplerState(SHADERSTAGES stage, SamplerState & state, U32 slot)
{
	auto rhiState = GetGraphicsDeviceChildState<SamplerStateD3D11>(state);
	if (rhiState == nullptr) {
		return;
	}

	ID3D11SamplerState* samplerState = rhiState->mHandle.Get();
	switch (stage)
	{
	case Cjing3D::SHADERSTAGES_VS:
		GetDeviceContext(GraphicsThread_IMMEDIATE).VSSetSamplers(slot, 1, &samplerState);
		break;
	case Cjing3D::SHADERSTAGES_GS:
		GetDeviceContext(GraphicsThread_IMMEDIATE).GSSetSamplers(slot, 1, &samplerState);
		break;
	case Cjing3D::SHADERSTAGES_HS:
		GetDeviceContext(GraphicsThread_IMMEDIATE).HSSetSamplers(slot, 1, &samplerState);
		break;
	case Cjing3D::SHADERSTAGES_DS:
		GetDeviceContext(GraphicsThread_IMMEDIATE).DSSetSamplers(slot, 1, &samplerState);
		break;
	case Cjing3D::SHADERSTAGES_PS:
		GetDeviceContext(GraphicsThread_IMMEDIATE).PSSetSamplers(slot, 1, &samplerState);
		break;
	case Cjing3D::SHADERSTAGES_CS:
		GetDeviceContext(GraphicsThread_IMMEDIATE).CSSetSamplers(slot, 1, &samplerState);
		break;
	default:
		break;
	}
}

// 创建2D纹理
HRESULT GraphicsDeviceD3D11::CreateTexture2D(const TextureDesc * desc, const SubresourceData * data, RhiTexture2D & texture2D)
{
	auto rhiState = RegisterGraphicsDeviceChild<TextureD3D11>(texture2D);
	texture2D.mCurrType = GPU_RESOURCE_TYPE::TEXTURE_2D;
	texture2D.mDesc = *desc;

	D3D11_TEXTURE2D_DESC tex2D_desc = _ConvertTexture2DDesc(desc);

	// texture initial data
	std::vector<D3D11_SUBRESOURCE_DATA> subresource_datas;
	if (data != nullptr)
	{
		U32 count = std::max(1u, desc->mArraySize * desc->mMipLevels);
		subresource_datas.resize(count);
		for (int i = 0; i < count; i++)	{
			subresource_datas[i] = _ConvertSubresourceData(data[i]);
		}
	}

	const auto result = mDevice->CreateTexture2D(&tex2D_desc, subresource_datas.data(), (ID3D11Texture2D**)rhiState->mResource.ReleaseAndGetAddressOf());
	if (FAILED(result)) {
		return result; 
	}

	// create graphics resource
	CreateRenderTargetView(texture2D);
	CreateShaderResourceView(texture2D);
	CreateDepthStencilView(texture2D);
	CreateUnordereddAccessView(texture2D);

	return result;
}

void GraphicsDeviceD3D11::CopyTexture2D(RhiTexture2D * texDst, RhiTexture2D * texSrc)
{
	auto dstRhiState = GetGraphicsDeviceChildState<TextureD3D11>(*texDst);
	if (dstRhiState == nullptr) {
		return;
	}

	auto srcRhiState = GetGraphicsDeviceChildState<TextureD3D11>(*texSrc);
	if (srcRhiState == nullptr) {
		return;
	}

	ID3D11Resource* dstResource = dstRhiState->mResource.Get();
	ID3D11Resource* srcResource = srcRhiState->mResource.Get();
	GetDeviceContext(GraphicsThread_IMMEDIATE).CopyResource(dstResource, srcResource);
}

void GraphicsDeviceD3D11::BindRenderTarget(UINT numView, RhiTexture2D* const *texture2D, RhiTexture2D* depthStencilTexture, I32 subresourceIndex)
{
	// get renderTargetView
	ID3D11RenderTargetView* renderTargetViews[8] = {};
	UINT curNum = std::min((int)numView, 8);
	for (int i = 0; i < curNum; i++) {
		if (texture2D[i] == nullptr) {
			continue;
		}

		auto rhiState = GetGraphicsDeviceChildState<TextureD3D11>(*texture2D[i]);
		if (rhiState == nullptr) {
			continue;
		}

		renderTargetViews[i] = rhiState->mRTV.Get();
	}

	// get depthStencilView
	ID3D11DepthStencilView* depthStencilView = nullptr;
	if (depthStencilTexture != nullptr) {
		auto rhiState = GetGraphicsDeviceChildState<TextureD3D11>(*depthStencilTexture);
		if (rhiState != nullptr) {
			depthStencilView = (subresourceIndex < 0) ? rhiState->mDSV.Get() : rhiState->mSubresourceDSVs[subresourceIndex].Get();
		}
	}

	GetDeviceContext(GraphicsThread_IMMEDIATE).OMSetRenderTargets(curNum, renderTargetViews, depthStencilView);
}

I32 GraphicsDeviceD3D11::CreateRenderTargetView(RhiTexture2D & texture)
{
	auto rhiState = GetGraphicsDeviceChildState<TextureD3D11>(texture);
	if (rhiState == nullptr) {
		return -1;
	}

	I32 subresourceIndex = -1;
	const auto& desc = texture.GetDesc();
	if (desc.mBindFlags & BIND_RENDER_TARGET)
	{
		U32 arraySize = desc.mArraySize;

		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc = {};
		renderTargetViewDesc.Format = _ConvertFormat(desc.mFormat);
		renderTargetViewDesc.Texture2DArray.MipSlice = 0;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

		// 立方体贴图
		if (desc.mMiscFlags & RESOURCE_MISC_TEXTURECUBE)
		{
		}
		else
		{
			// 目前不处理texture2DArray,且不处理multisampled
			if (arraySize > 0 && arraySize <= 1)
			{
				ComPtr<ID3D11RenderTargetView> newRTV = nullptr;
				HRESULT result = mDevice->CreateRenderTargetView(rhiState->mResource.Get(), &renderTargetViewDesc, &newRTV);
				if (SUCCEEDED(result))
				{
					if (rhiState->mRTV == nullptr) 
					{
						rhiState->mRTV = newRTV;
					}
					else
					{
						//Debug::Warning("CreateShaderResourceView: create new subresource, count:" + std::to_string(texture.mSubresourceSRVs.size()));
						subresourceIndex = rhiState->mSubresourceRTVs.size();
						rhiState->mSubresourceRTVs.push_back(newRTV);
					}
				}
			}
		}
	}

	return subresourceIndex;
}

I32 GraphicsDeviceD3D11::CreateShaderResourceView(RhiTexture2D & texture, U32 arraySlice, U32 arrayCount, U32 firstMip, U32 mipLevel)
{
	auto rhiState = GetGraphicsDeviceChildState<TextureD3D11>(texture);
	if (rhiState == nullptr) {
		return -1;
	}

	I32 subresourceIndex = -1;
	const auto& desc = texture.GetDesc();
	if (desc.mBindFlags & BIND_SHADER_RESOURCE)
	{
		U32 arraySize = desc.mArraySize;

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
		switch (desc.mFormat)
		{
		case FORMAT_R16_TYPELESS:
			shaderResourceViewDesc.Format = DXGI_FORMAT_R16_UNORM;
			break;
		case FORMAT_R32G8X24_TYPELESS:
			shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
			break;
		default:
			shaderResourceViewDesc.Format = _ConvertFormat(desc.mFormat);
			break;
		}

		if (arraySize > 0 && arraySize <= 1)
		{
			shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			shaderResourceViewDesc.Texture2D.MostDetailedMip = firstMip;   // 开始使用的miplevel
			shaderResourceViewDesc.Texture2D.MipLevels = mipLevel;         // 使用mipLevel数量
		}
		else
		{
			if (desc.mMiscFlags & RESOURCE_MISC_TEXTURECUBE)
			{
				if (arraySize > 6)
				{
					shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
					shaderResourceViewDesc.TextureCubeArray.First2DArrayFace = arraySlice;
					shaderResourceViewDesc.TextureCubeArray.NumCubes = std::min(desc.mArraySize, arrayCount) / 6;
					shaderResourceViewDesc.TextureCubeArray.MostDetailedMip = firstMip;
					shaderResourceViewDesc.TextureCubeArray.MipLevels = mipLevel;
				}
				else
				{
					shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
					shaderResourceViewDesc.TextureCube.MostDetailedMip = firstMip;
					shaderResourceViewDesc.TextureCube.MipLevels = mipLevel;
				}
			}
			else
			{
				shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
				shaderResourceViewDesc.Texture2DArray.FirstArraySlice = arraySlice; // 使用的texture的索引
				shaderResourceViewDesc.Texture2DArray.ArraySize = arraySize;        // texture的数量
				shaderResourceViewDesc.Texture2DArray.MostDetailedMip = firstMip;   // 开始使用的miplevel
				shaderResourceViewDesc.Texture2DArray.MipLevels = mipLevel;         // 使用mipLevel数量
			}
		}

		ComPtr<ID3D11ShaderResourceView> newSRV = nullptr;
		auto result = mDevice->CreateShaderResourceView(rhiState->mResource.Get(), &shaderResourceViewDesc, &newSRV);
		if (SUCCEEDED(result))
		{
			if (rhiState->mSRV == nullptr)
			{
				rhiState->mSRV = newSRV;
			}
			else 
			{
				//Debug::Warning("CreateShaderResourceView: create new subresource, count:" + std::to_string(texture.mSubresourceSRVs.size()));
				subresourceIndex = rhiState->mSubresourceSRVs.size();
				rhiState->mSubresourceSRVs.push_back(newSRV);
			}
		}
	}

	return subresourceIndex;
}

// arraySlice和arrayCount仅用于当texture.ArraySize > 0时使用
// arraySlice表示textureArray的第几部分，arrayCount表示这部分的数量
I32 GraphicsDeviceD3D11::CreateDepthStencilView(RhiTexture2D & texture, U32 arraySlice, U32 arrayCount)
{
	auto rhiState = GetGraphicsDeviceChildState<TextureD3D11>(texture);
	if (rhiState == nullptr) {
		return -1;
	}

	I32 subresourceIndex = -1;
	const auto& desc = texture.GetDesc();
	if (desc.mBindFlags & BIND_DEPTH_STENCIL)
	{
		U32 arraySize = desc.mArraySize;

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
		switch (desc.mFormat)
		{
		case FORMAT_R16_TYPELESS:
			depthStencilViewDesc.Format = DXGI_FORMAT_D16_UNORM;
			break;
		case FORMAT_R32G8X24_TYPELESS:
			depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
			break;
		default:
			depthStencilViewDesc.Format = _ConvertFormat(desc.mFormat);
			break;
		}

		if (arraySize > 0 && arraySize <= 1)
		{
			depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			depthStencilViewDesc.Texture2D.MipSlice = 0;
		}
		else
		{
			depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			depthStencilViewDesc.Texture2DArray.FirstArraySlice = arraySlice;
			depthStencilViewDesc.Texture2DArray.ArraySize = arrayCount;
			depthStencilViewDesc.Texture2DArray.MipSlice = 0;
		}

		ComPtr<ID3D11DepthStencilView> newDsv = nullptr;
		HRESULT result = mDevice->CreateDepthStencilView(rhiState->mResource.Get(), &depthStencilViewDesc, &newDsv);
		if (SUCCEEDED(result))
		{
			if (rhiState->mDSV == nullptr) 
			{
				rhiState->mDSV = newDsv;
			}
			else 
			{
				subresourceIndex = rhiState->mSubresourceDSVs.size();
				rhiState->mSubresourceDSVs.push_back(newDsv);
			}
		}
	}

	return subresourceIndex;
}

I32 GraphicsDeviceD3D11::CreateUnordereddAccessView(RhiTexture2D& texture, U32 firstMip)
{
	auto rhiState = GetGraphicsDeviceChildState<TextureD3D11>(texture);
	if (rhiState == nullptr) {
		return -1;
	}

	I32 subresourceIndex = -1;
	const auto& desc = texture.GetDesc();
	if (desc.mBindFlags & BIND_UNORDERED_ACCESS)
	{
		U32 arraySize = desc.mArraySize;

		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		switch (desc.mFormat)
		{
		case FORMAT_R32G8X24_TYPELESS:
			uavDesc.Format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
			break;
		default:
			uavDesc.Format = _ConvertFormat(desc.mFormat);
			break;
		}

		// 目前不处理texture2DArray
		if (arraySize > 0 && arraySize <= 1)
		{
			uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
			uavDesc.Texture2D.MipSlice = firstMip;

			ComPtr <ID3D11UnorderedAccessView> newUAV = nullptr;
			HRESULT result = mDevice->CreateUnorderedAccessView(rhiState->mResource.Get(), &uavDesc, &newUAV);
			if (SUCCEEDED(result))
			{
				if (rhiState->mUAV == nullptr)
				{
					rhiState->mUAV = newUAV;
				}
				else 
				{
					//Debug::Warning("CreateUnordereddAccessView: create new subresource, count:" + std::to_string(texture.mSubresourceUAVS.size()));
					subresourceIndex = rhiState->mSubresourceUAVS.size();
					rhiState->mSubresourceUAVS.push_back(newUAV);
				}
			}
		}
	}

	return subresourceIndex;
}

void GraphicsDeviceD3D11::ClearRenderTarget(RhiTexture2D & texture, F32x4 color)
{
	auto rhiState = GetGraphicsDeviceChildState<TextureD3D11>(texture);
	if (rhiState == nullptr) {
		return;
	}

	GetDeviceContext(GraphicsThread_IMMEDIATE).ClearRenderTargetView(rhiState->mRTV.Get(), color.data());
}

void GraphicsDeviceD3D11::ClearDepthStencil(RhiTexture2D& texture, UINT clearFlag, F32 depth, U8 stencil, I32 subresourceIndex)
{
	auto rhiState = GetGraphicsDeviceChildState<TextureD3D11>(texture);
	if (rhiState == nullptr) {
		return;
	}

	UINT flag = _ConvertClearFlag(clearFlag);
	if (subresourceIndex < 0) {
		GetDeviceContext(GraphicsThread_IMMEDIATE).ClearDepthStencilView(rhiState->mDSV.Get(), flag, depth, stencil);
	}
	else {
		ID3D11DepthStencilView* dsv = rhiState->mSubresourceDSVs[subresourceIndex].Get();
		GetDeviceContext(GraphicsThread_IMMEDIATE).ClearDepthStencilView(dsv, flag, depth, stencil);
	}
}

void GraphicsDeviceD3D11::BindGPUResource(SHADERSTAGES stage, GPUResource& resource, U32 slot, I32 subresourceIndex)
{
	auto rhiState = GetGraphicsDeviceChildState<GPUResourceD3D11>(resource);
	if (rhiState == nullptr) {
		Debug::Warning("Faild to bind null gpu resource");
		return;
	}

	ID3D11ShaderResourceView* srv = nullptr;
	if (subresourceIndex < 0) {
		srv = (ID3D11ShaderResourceView*)rhiState->mSRV.Get();
	}
	else if (subresourceIndex < rhiState->mSubresourceSRVs.size()) {
		srv = (ID3D11ShaderResourceView*)rhiState->mSubresourceSRVs[subresourceIndex].Get();
	}
		
	if (srv == nullptr) {
		Debug::Warning("BindGPUResource: Invalid gpu resource");
		return;
	}
		
	switch (stage)
	{
	case Cjing3D::SHADERSTAGES_VS:
		GetDeviceContext(GraphicsThread_IMMEDIATE).VSSetShaderResources(slot, 1, &srv);
		break;
	case Cjing3D::SHADERSTAGES_GS:
		GetDeviceContext(GraphicsThread_IMMEDIATE).GSSetShaderResources(slot, 1, &srv);
		break;
	case Cjing3D::SHADERSTAGES_HS:
		GetDeviceContext(GraphicsThread_IMMEDIATE).HSSetShaderResources(slot, 1, &srv);
		break;
	case Cjing3D::SHADERSTAGES_DS:
		GetDeviceContext(GraphicsThread_IMMEDIATE).DSSetShaderResources(slot, 1, &srv);
		break;
	case Cjing3D::SHADERSTAGES_PS:
		GetDeviceContext(GraphicsThread_IMMEDIATE).PSSetShaderResources(slot, 1, &srv);
		break;
	case Cjing3D::SHADERSTAGES_CS:
		GetDeviceContext(GraphicsThread_IMMEDIATE).CSSetShaderResources(slot, 1, &srv);
		break;
	default:
		break;
	}
}

void GraphicsDeviceD3D11::BindGPUResources(SHADERSTAGES stage, GPUResource * const * resource, U32 slot, U32 count)
{
	ID3D11ShaderResourceView* srvs[8] = { nullptr };
	for (int i = 0; i < count; i++) 
	{
		if (resource[i] == nullptr) {
			continue;
		}

		auto rhiState = GetGraphicsDeviceChildState<GPUResourceD3D11>(*resource[i]);
		if (rhiState == nullptr) {
			continue;
		}

		srvs[i] = rhiState->mSRV.Get();
	}
	switch (stage)
	{
	case Cjing3D::SHADERSTAGES_VS:
		GetDeviceContext(GraphicsThread_IMMEDIATE).VSSetShaderResources(slot, count, srvs);
		break;
	case Cjing3D::SHADERSTAGES_GS:
		GetDeviceContext(GraphicsThread_IMMEDIATE).GSSetShaderResources(slot, count, srvs);
		break;
	case Cjing3D::SHADERSTAGES_HS:
		GetDeviceContext(GraphicsThread_IMMEDIATE).HSSetShaderResources(slot, count, srvs);
		break;
	case Cjing3D::SHADERSTAGES_DS:
		GetDeviceContext(GraphicsThread_IMMEDIATE).DSSetShaderResources(slot, count, srvs);
		break;
	case Cjing3D::SHADERSTAGES_PS:
		GetDeviceContext(GraphicsThread_IMMEDIATE).PSSetShaderResources(slot, count, srvs);
		break;
	case Cjing3D::SHADERSTAGES_CS:
		GetDeviceContext(GraphicsThread_IMMEDIATE).CSSetShaderResources(slot, count, srvs);
		break;
	default:
		break;
	}
}

void GraphicsDeviceD3D11::UnbindGPUResources(U32 slot, U32 count)
{
	GetDeviceContext(GraphicsThread_IMMEDIATE).VSSetShaderResources(slot, count, (ID3D11ShaderResourceView**)nullptrBlob);
	GetDeviceContext(GraphicsThread_IMMEDIATE).GSSetShaderResources(slot, count, (ID3D11ShaderResourceView**)nullptrBlob);
	GetDeviceContext(GraphicsThread_IMMEDIATE).PSSetShaderResources(slot, count, (ID3D11ShaderResourceView**)nullptrBlob);
	GetDeviceContext(GraphicsThread_IMMEDIATE).CSSetShaderResources(slot, count, (ID3D11ShaderResourceView**)nullptrBlob);
}

void GraphicsDeviceD3D11::SetResourceName(GPUResource & resource, const std::string & name)
{
	auto rhiState = GetGraphicsDeviceChildState<GPUResourceD3D11>(resource);
	if (rhiState == nullptr) {
		return;
	}

	ID3D11Resource* d3dResource = rhiState->mResource.Get();
	d3dResource->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)name.length(), name.c_str());
}

void GraphicsDeviceD3D11::CreateRenderBehavior(RenderBehaviorDesc& desc, RenderBehavior& behavior)
{
	behavior.mRhiState = mEmptyRhiState;
	behavior.mDesc = desc;
}

void GraphicsDeviceD3D11::BeginRenderBehavior(RenderBehavior& behavior)
{
	ID3D11RenderTargetView* renderTargetViews[8] = {};
	UINT rtCount = 0;
	ID3D11DepthStencilView* depthStencilView = nullptr;

	// 遍历所有RenderBehaviorParams填充rtvs和dsv
	RenderBehaviorDesc& desc = behavior.mDesc;
	for (RenderBehaviorParam& param : desc.mParams)
	{
		const Texture* texture = param.mTexture;
		if (texture == nullptr) {
			continue;
		}

		auto rhiState = GetGraphicsDeviceChildState<TextureD3D11>(*texture);
		if (rhiState == nullptr) {
			continue;
		}

		I32 subresourceIndex = param.mSubresourceIndex;
		if (param.mType == RenderBehaviorParam::RenderType_RenderTarget)
		{	
			Debug::CheckAssertion(subresourceIndex < (I32)rhiState->mSubresourceRTVs.size(), "Invalid renderTarget subresource.");
			renderTargetViews[rtCount] = (subresourceIndex < 0) ? rhiState->mRTV.Get() : rhiState->mSubresourceRTVs[subresourceIndex].Get();

			if (param.mOperation == RenderBehaviorParam::RenderOperation_Clear) {
				ClearValue value = texture->mDesc.mClearValue;
				GetDeviceContext(GraphicsThread_IMMEDIATE).ClearRenderTargetView(renderTargetViews[rtCount], texture->mDesc.mClearValue.color);
			}

			rtCount++;
		}
		else if (param.mType == RenderBehaviorParam::RenderType_DepthStencil)
		{
			Debug::CheckAssertion(subresourceIndex < (I32)rhiState->mSubresourceDSVs.size(), "Invalid renderTarget subresource.");
			depthStencilView = (subresourceIndex < 0) ? rhiState->mDSV.Get() : rhiState->mSubresourceDSVs[subresourceIndex].Get();
		
			if (param.mOperation == RenderBehaviorParam::RenderOperation_Clear) {
				uint32_t flag = D3D11_CLEAR_DEPTH;
				if (IsFormatStencilSupport(texture->mDesc.mFormat)) {
					flag |= D3D11_CLEAR_STENCIL;
				}

				ClearValue value = texture->mDesc.mClearValue;
				GetDeviceContext(GraphicsThread_IMMEDIATE).ClearDepthStencilView(depthStencilView, flag, value.depth, value.stencil);
			}
		}
	}

	GetDeviceContext(GraphicsThread_IMMEDIATE).OMSetRenderTargets(rtCount, renderTargetViews, depthStencilView);
}

void GraphicsDeviceD3D11::EndRenderBehavior()
{
	GetDeviceContext(GraphicsThread_IMMEDIATE).OMSetRenderTargets(0, nullptr, nullptr);
}

void GraphicsDeviceD3D11::BindComputeShader(ShaderPtr computeShader)
{
	ID3D11ComputeShader* cs = nullptr;
	if (computeShader != nullptr)
	{
		auto rhiState = GetGraphicsDeviceChildState<ComputeShaderD3D11>(*computeShader);
		if (rhiState != nullptr) {
			cs = rhiState->mHandle.Get();
		}
	}

	if (cs != mPrevComputeShader)
	{
		GetDeviceContext(GraphicsThread_IMMEDIATE).CSSetShader(cs, nullptr, 0);
		mPrevComputeShader = cs;
	}
}

void GraphicsDeviceD3D11::Dispatch(U32 threadGroupCountX, U32 threadGroupCountY, U32 threadGroupCountZ)
{
	GetDeviceContext(GraphicsThread_IMMEDIATE).Dispatch(threadGroupCountX, threadGroupCountY, threadGroupCountZ);
}

void GraphicsDeviceD3D11::UnBindUAVs(U32 slot, U32 count)
{
	Debug::CheckAssertion(count <= ARRAYSIZE(nullptrBlob), "GraphicsDeviceD3D11::UnBindUAVs: Invalid count.");
	GetDeviceContext(GraphicsThread_IMMEDIATE).CSSetUnorderedAccessViews(slot, count, (ID3D11UnorderedAccessView**)nullptrBlob, nullptr);
}

void GraphicsDeviceD3D11::BindUAV(GPUResource* const resource, U32 slot, I32 subresourceIndex)
{
	if (resource == nullptr) return;

	auto rhiState = GetGraphicsDeviceChildState<GPUResourceD3D11>(*resource);
	if (rhiState == nullptr) {
		Debug::Warning("Faild to bind null uav resource");
		return;
	}

	ID3D11UnorderedAccessView* uav = nullptr;
	if (subresourceIndex < 0) {
		uav = (ID3D11UnorderedAccessView*)rhiState->mUAV.Get();
	}
	else if (subresourceIndex < rhiState->mSubresourceUAVS.size()) {
		uav = (ID3D11UnorderedAccessView*)rhiState->mSubresourceUAVS[subresourceIndex].Get();
	}

	if (uav == nullptr) {
		Debug::Warning("BindUAV: Invalid gpu resource");
		return;
	}

	GetDeviceContext(GraphicsThread_IMMEDIATE).CSSetUnorderedAccessViews(slot, 1, &uav, nullptr);
}

void GraphicsDeviceD3D11::BindUAVs(GPUResource* const* resource, U32 slot, U32 count)
{
	ID3D11UnorderedAccessView* uavs[8] = { nullptr };
	for (int i = 0; i < count; i++)
	{
		if (i >= 8) break;
	
		if (resource[i] == nullptr) {
			continue;
		}

		auto rhiState = GetGraphicsDeviceChildState<GPUResourceD3D11>(*resource[i]);
		if (rhiState == nullptr) {
			continue;
		}

		uavs[i] = rhiState->mUAV.Get();
	}

	GetDeviceContext(GraphicsThread_IMMEDIATE).CSSetUnorderedAccessViews(slot, count, uavs, nullptr);
}

void GraphicsDeviceD3D11::BindPipelineState(PipelineState state)
{
	PipelineStateDesc& desc = state.mDesc;

	ID3D11VertexShader* vs = desc.mVertexShader != nullptr ?
		GetGraphicsDeviceChildState<VertexShaderD3D11>(*desc.mVertexShader)->mHandle.Get() : nullptr;
	if (vs != mPrevVertexShader)
	{
		GetDeviceContext(GraphicsThread_IMMEDIATE).VSSetShader(vs, nullptr, 0);
		mPrevVertexShader = vs;
	}

	ID3D11PixelShader* ps = desc.mPixelShader != nullptr ? 
		GetGraphicsDeviceChildState<PixelShaderD3D11>(*desc.mPixelShader)->mHandle.Get() : nullptr;
	if (ps != mPrevPixelShader)
	{
		GetDeviceContext(GraphicsThread_IMMEDIATE).PSSetShader(ps, nullptr, 0);
		mPrevPixelShader = ps;
	}

	ID3D11HullShader* hs = desc.mHullShader != nullptr ? 
		GetGraphicsDeviceChildState<HullShaderD3D11>(*desc.mHullShader)->mHandle.Get() : nullptr;
	if (hs != mPrevHullShader)
	{
		GetDeviceContext(GraphicsThread_IMMEDIATE).HSSetShader(hs, nullptr, 0);
		mPrevHullShader = hs;
	}

	ID3D11DomainShader* ds = desc.mDomainShader != nullptr ? 
		GetGraphicsDeviceChildState<DomainShaderD3D11>(*desc.mDomainShader)->mHandle.Get() : nullptr;
	if (ds != mPrevDomainShader)
	{
		GetDeviceContext(GraphicsThread_IMMEDIATE).DSSetShader(ds, nullptr, 0);
		mPrevDomainShader = ds;
	}

	ID3D11BlendState* bs = desc.mBlendState != nullptr ? 
		GetGraphicsDeviceChildState<BlendStateD3D11>(*desc.mBlendState)->mHandle.Get() : nullptr;
	if (bs != mPrevBlendState)
	{
		const float factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		GetDeviceContext(GraphicsThread_IMMEDIATE).OMSetBlendState(bs, factor, 0xFFFFFFFF);
		mPrevBlendState = bs;
	}

	ID3D11RasterizerState* rs = desc.mRasterizerState != nullptr ? 
		GetGraphicsDeviceChildState<RasterizerStateD3D11>(*desc.mRasterizerState)->mHandle.Get() : nullptr;
	if (rs != mPrevRasterizerState)
	{
		GetDeviceContext(GraphicsThread_IMMEDIATE).RSSetState(rs);
		mPrevRasterizerState = rs;
	}

	ID3D11DepthStencilState* dss = desc.mDepthStencilState != nullptr ? 
		GetGraphicsDeviceChildState<DepthStencilStateD3D11>(*desc.mDepthStencilState)->mHandle.Get() : nullptr;
	if (dss != mPrevDepthStencilState)
	{
		GetDeviceContext(GraphicsThread_IMMEDIATE).OMSetDepthStencilState(dss, 0);
		mPrevDepthStencilState = dss;
	}

	ID3D11InputLayout* il = desc.mInputLayout != nullptr ? 
		GetGraphicsDeviceChildState<InputLayoutD3D11>(*desc.mInputLayout)->mHandle.Get() : nullptr;
	if (il != mPrevInputLayout)
	{
		GetDeviceContext(GraphicsThread_IMMEDIATE).IASetInputLayout(il);
		mPrevInputLayout = il;
	}

	if (desc.mPrimitiveTopology != mPrevPrimitiveTopology)
	{
		D3D11_PRIMITIVE_TOPOLOGY primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		switch (desc.mPrimitiveTopology)
		{
		case TRIANGLELIST:
			primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			break;
		case TRIANGLESTRIP:
			primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
			break;
		case POINTLIST:
			primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
			break;
		case LINELIST:
			primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
			break;
		case PATCHLIST_3:
			primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
			break;
		case PATCHLIST_4:
			primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;
			break;
		default:
			primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
			break;
		}

		GetDeviceContext(GraphicsThread_IMMEDIATE).IASetPrimitiveTopology(primitiveTopology);
		mPrevPrimitiveTopology = desc.mPrimitiveTopology;
	}
}

void GraphicsDeviceD3D11::SetViewport(ViewPort viewport)
{
	mViewport = viewport;
}

GraphicsDevice::GPUAllocation GraphicsDeviceD3D11::AllocateGPU(size_t dataSize)
{
	// 分配mGPUAllocator的buffer内存
	GPUAllocation result;

	if (dataSize == 0) {
		return result;
	}

	auto rhiState = GetGraphicsDeviceChildState<GPUResourceD3D11>(mGPUAllocator.buffer);
	if (rhiState == nullptr) {
		return result;
	}

	size_t allocDataSize = std::min(mGPUAllocator.GetDataSize(), dataSize);
	size_t position = mGPUAllocator.byteOffset;

	// 如果分配的大小超过了最大大小或者分配时处于新的一帧，则覆盖
	bool wrap = false;
	if (position + dataSize > mGPUAllocator.GetDataSize() ||
		mGPUAllocator.residentFrame != mCurrentFrameCount) {
		wrap = true;
	}
	position = wrap ? 0 : position;

	// mapping data
	D3D11_MAP mapping = wrap ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_WRITE_NO_OVERWRITE;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	const HRESULT hr = GetDeviceContext(GraphicsThread_IMMEDIATE).Map(rhiState->mResource.Get(), 0, mapping, 0, &mappedResource);
	Debug::ThrowIfFailed(hr, "Failed to mapped GPUBuffer.");

	mGPUAllocator.residentFrame = mCurrentFrameCount;
	mGPUAllocator.byteOffset = position + dataSize;
	mGPUAllocator.dirty = true;

	result.buffer = &mGPUAllocator.buffer;
	result.offset = (U32)position;
	result.data = (void*)((size_t)mappedResource.pData + position);

	return result;
}

void GraphicsDeviceD3D11::UnAllocateGPU()
{
	CommitAllocations();
}

void GraphicsDeviceD3D11::ClearPrevStates()
{
	mPrevVertexShader = nullptr;
	mPrevPixelShader = nullptr;
	mPrevComputeShader = nullptr;
	mPrevHullShader = nullptr;
	mPrevDomainShader = nullptr;
	mPrevInputLayout = nullptr;
	mPrevRasterizerState = nullptr;
	mPrevDepthStencilState = nullptr;
	mPrevBlendState = nullptr;

	mPrevPrimitiveTopology = UNDEFINED_TOPOLOGY;
}

void GraphicsDeviceD3D11::Draw(UINT vertexCount, UINT startVertexLocation)
{
	CommitAllocations();

	GetDeviceContext(GraphicsThread_IMMEDIATE).Draw(vertexCount, startVertexLocation);
}

void GraphicsDeviceD3D11::DrawIndexed(UINT indexCount, UINT startIndexLocation)
{
	CommitAllocations();

	GetDeviceContext(GraphicsThread_IMMEDIATE).DrawIndexed(indexCount, startIndexLocation, 0);
}

void GraphicsDeviceD3D11::DrawIndexedInstances(U32 indexCount, U32 instanceCount, U32 startIndexLocation, U32 baseVertexLocation, U32 startInstanceLocation)
{
	CommitAllocations();

	GetDeviceContext(GraphicsThread_IMMEDIATE).DrawIndexedInstanced(
		indexCount,
		instanceCount,
		startIndexLocation,
		baseVertexLocation,
		startInstanceLocation);
}

HRESULT GraphicsDeviceD3D11::CreateQuery(const GPUQueryDesc& desc, GPUQuery& query)
{
	query.mDesc = desc;

	D3D11_QUERY_DESC queryDesc = {};
	queryDesc.MiscFlags = 0;
	queryDesc.Query = D3D11_QUERY_EVENT;

	// 时间戳
	if (desc.mGPUQueryType == GPU_QUERY_TYPE_TIMESTAMP)
	{
		queryDesc.Query = D3D11_QUERY_TIMESTAMP;
	}
	// 始终频率
	else if (desc.mGPUQueryType == GPU_QUERY_TYPE_TIMESTAMP_DISJOINT)
	{
		queryDesc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
	}

	auto rhiState = RegisterGraphicsDeviceChild<QueryD3D11>(query);
	return mDevice->CreateQuery(&queryDesc, &rhiState->mHandle);
}

void GraphicsDeviceD3D11::BeginQuery(GPUQuery& query)
{
	auto rhiState = GetGraphicsDeviceChildState<QueryD3D11>(query);
	if (rhiState == nullptr) {
		return;
	}
	GetDeviceContext(GraphicsThread_IMMEDIATE).Begin(rhiState->mHandle.Get());
}

void GraphicsDeviceD3D11::EndQuery(GPUQuery& query)
{
	auto rhiState = GetGraphicsDeviceChildState<QueryD3D11>(query);
	if (rhiState == nullptr) {
		return;
	}
	GetDeviceContext(GraphicsThread_IMMEDIATE).End(rhiState->mHandle.Get());
}

HRESULT GraphicsDeviceD3D11::ReadQuery(GPUQuery& query, GPUQueryResult& result)
{
	auto rhiState = GetGraphicsDeviceChildState<QueryD3D11>(query);
	if (rhiState == nullptr) {
		return S_OK;
	}

	HRESULT hr = S_OK;
	GPU_QUERY_TYPE type = query.mDesc.mGPUQueryType;
	switch (type)
	{
	case GPU_QUERY_TYPE_TIMESTAMP:
		GetDeviceContext(GraphicsThread_IMMEDIATE).GetData(rhiState->mHandle.Get(), &result.mTimestamp, sizeof(U64), D3D11_ASYNC_GETDATA_DONOTFLUSH);
		break;
	case GPU_QUERY_TYPE_TIMESTAMP_DISJOINT:
		D3D11_QUERY_DATA_TIMESTAMP_DISJOINT disjointData;
		GetDeviceContext(GraphicsThread_IMMEDIATE).GetData(rhiState->mHandle.Get(), &disjointData, sizeof(disjointData), D3D11_ASYNC_GETDATA_DONOTFLUSH);
		result.mTimetampFrequency = disjointData.Frequency;
		break;
	default:
		break;
	}

	return hr;
}

void GraphicsDeviceD3D11::InitializeDevice()
{
	// 优先D3D_DRIVER_TYPE_HARDWARE
	const static D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	U32 numTypes = ARRAYSIZE(driverTypes);

	UINT createDeviceFlags = 0;
	if (mDebugLayer == true) {
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	}

	const static D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	for (int index = 0; index < numTypes; index++)
	{
		auto driverType = driverTypes[index];

		ComPtr<ID3D11Device> device;
		ComPtr<ID3D11DeviceContext> deviceContext;

		const HRESULT result = D3D11CreateDevice(
			nullptr,
			driverType,
			nullptr,
			createDeviceFlags,
			featureLevels,
			numFeatureLevels,
			D3D11_SDK_VERSION,
			device.GetAddressOf(),
			&mFeatureLevel,
			deviceContext.GetAddressOf());
		
		if (SUCCEEDED(result)) {
			device.As(&mDevice);
			deviceContext.As(&(mDeviceContext[GraphicsThread_IMMEDIATE]));

			break;
		}
	}

	if (mDevice == nullptr || mDeviceContext == nullptr)
	{
		Debug::Error("Failed to initialize d3d device.");
		return;
	}
}

void GraphicsDeviceD3D11::CommitAllocations()
{
	// 在执行操作前（例如draw)需要unmap buffer
	if (mGPUAllocator.dirty)
	{
		auto& buffer = mGPUAllocator.buffer;
		auto rhiState = GetGraphicsDeviceChildState<GPUResourceD3D11>(buffer);
		if (rhiState != nullptr) {
			GetDeviceContext(GraphicsThread_IMMEDIATE).Unmap(rhiState->mResource.Get(), 0);
		}

		mGPUAllocator.dirty = false;
	}
}

}
