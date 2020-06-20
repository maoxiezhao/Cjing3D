#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\renderer.h"
#include "renderer\RHI\rhiResource.h"
#include "renderer\RHI\rhiShader.h"

namespace Cjing3D
{

class MaterialComponent;

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
	VertexShaderType_Count,
};

enum InputLayoutType
{
	InputLayoutType_ObjectAll = 0,
	InputLayoutType_ObjectPosTex,
	InputLayoutType_ObjectPos,
	InputLayoutType_Shadow,
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
	PixelShaderType_Count,
};

enum ComputeShaderType
{
	ComputeShaderType_Tonemapping = 0,
	ComputeShaderType_MipmapGenerate,
	ComputeShaderType_FXAA,
	ComputeShaderType_Skinning,
	ComputeShaderType_TiledFrustum,
	ComputeShaderType_LightTiledCulling,
	ComputeShaderType_Count
};

class ShaderLib
{
public:
	ShaderLib();

	void Initialize();
	void Uninitialize();

	std::shared_ptr<InputLayout>   GetVertexLayout(InputLayoutType layoutType);
	std::shared_ptr<Shader>   GetVertexShader(VetextShaderType shaderType);
	std::shared_ptr<Shader>   GetPixelShader(PixelShaderType shaderType);
	std::shared_ptr<Shader>   GetComputeShader(ComputeShaderType shaderType);

	ShaderPtr LoadShader(SHADERSTAGES stages, const std::string& path);
	VertexShaderInfo LoadVertexShaderInfo(const std::string& path, VertexLayoutDesc* desc, U32 numElements);

private:
	void LoadVertexShaders();
	void LoadPixelShaders();
	void LoadComputeShaders();

	InputLayoutPtr LoadInputLayout(ShaderPtr Shader, VertexLayoutDesc* desc, U32 numElements);
	ShaderPtr LoadShader(SHADERSTAGES stages, const void* data, size_t length);

private:
	std::shared_ptr<InputLayout> mInputLayout[InputLayoutType_Count];
	std::shared_ptr<Shader> mVertexShader[VertexShaderType_Count];
	std::shared_ptr<Shader> mPixelShader[PixelShaderType_Count];
	std::shared_ptr<Shader> mComputeShader[ComputeShaderType_Count];
};

class CustomShaderLib : public ShaderLib
{
public:
	CustomShaderLib();

	template<typename ShaderType>
	void RegisterShader();
};

}