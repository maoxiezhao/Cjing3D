#pragma once

namespace Cjing3D
{
	enum StructuredBufferType
	{
		StructuredBufferType_ShaderLight = 0,
		StructuredBufferType_MatrixArray,
		StructuredBufferType_Count,
	};

	enum ConstantBufferType
	{
		ConstantBufferType_Common = 0,
		ConstantBufferType_Camera,
		ConstantBufferType_Frame,
		ConstantBufferType_Image,
		ConstantBufferType_Postprocess,
		ConstantBufferType_MipmapGenerate,
		ConstantBufferType_CubeMap,
		ConstantBufferType_CSParams,
		ConstantBufferType_Misc,
		ConstantBufferType_Count,
	};

	// depthStencilState
	enum DepthStencilStateID {
		DepthStencilStateID_DepthNone = 0,
		DepthStencilStateID_GreaterEqualReadWrite,
		DepthStencilStateID_DepthRead,
		DepthStencilStateID_DepthReadEqual,
		DepthStencilStateID_Shadow,
		DepthStencilStateID_Count
	};

	// blend State
	enum BlendStateID {
		BlendStateID_Opaque = 0,
		BlendStateID_Transpranent,
		BlendStateID_PreMultiplied,
		BlendStateID_ColorWriteDisable,
		BlendStateID_Particle_Alpha,
		BlendStateID_Count
	};

	// Rasterizer State
	enum RasterizerStateID {
		RasterizerStateID_Front = 0,
		RasteruzerStateUD_Front_DoubleSided,
		RasterizerStateID_Image,
		RasterizerStateID_Shadow,
		RasterizerStateID_WireFrame,
		RasterizerStateID_Sky,
		RasterizerStateID_WireFrame_DoubleSided,
		RasterizerStateID_Particle,
		RasterizerStateID_Count
	};

	// Sampler State
	enum SamplerStateID {
		SamplerStateID_PointClamp,
		SamplerStateID_LinearClamp,
		SamplerStateID_Anisotropic,
		SamplerStateID_ComparisionDepth,
		SamplerStateID_Object,
		SamplerStateID_Font,
		SamplerStateID_Count
	};
}