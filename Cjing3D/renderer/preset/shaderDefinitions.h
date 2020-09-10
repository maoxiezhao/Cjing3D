#pragma once

namespace Cjing3D
{
	// TODO: 使用配置文件来配置

	enum VetextShaderType
	{
		VertexShaderType_ObjectAll = 0,
		VertexShaderType_ObjectPosTex,
		VertexShaderType_ObjectPos,
		VertexShaderType_FullScreen,
		VertexShaderType_Image,
		VertexShaderType_Shadow,
		VertexShaderType_ShadowCube,
		VertexShaderType_Sky,
		VertexShaderType_PosColor,
		VertexShaderType_Count,
	};

	enum InputLayoutType
	{
		InputLayoutType_ObjectAll = 0,
		InputLayoutType_ObjectPosTex,
		InputLayoutType_ObjectPos,
		InputLayoutType_Shadow,
		InputLayoutType_PosColor,
		InputLayoutType_Count
	};

	enum PixelShaderType
	{
		PixelShaderType_Object_Forward = 0,
		PixelShaderType_Object_Forward_Transparent,
		PixelShaderType_Object_TiledForward,
		PixelShaderType_Object_TiledForward_Transparent,
		PixelShaderType_Object_AlphaTest,
		PixelShaderType_FullScreen,
		PixelShaderType_Image,
		PixelShaderType_Sky,
		PixelShaderType_PosColor,
		PixelShaderType_Count,
	};

	enum ComputeShaderType
	{
		ComputeShaderType_Tonemapping = 0,
		ComputeShaderType_MipmapGenerate,
		ComputeShaderType_MipmapCubeGenerate,
		ComputeShaderType_MipmapGenerateUnorm,
		ComputeShaderType_MipmapCubeGenerateUnorm,
		ComputeShaderType_FXAA,
		ComputeShaderType_Skinning,
		ComputeShaderType_TiledFrustum,
		ComputeShaderType_LightTiledCulling,
		ComputeShaderType_LinearDepth,
		ComputeShaderType_SSAO,
		ComputeShaderType_GaussianBlur,
		ComputeShaderType_BilateralBlur,
		ComputeShaderType_BilateralUpsample,
		ComputeShaderType_Count
	};

}