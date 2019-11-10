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
	VertexShaderType_Transform = 0,
	VertexShaderType_FullScreen,
	VertexShaderType_Count,
};

enum InputLayoutType
{
	InputLayoutType_Transform = 0,
	InputLayoutType_Count
};

enum PixelShaderType
{
	PixelShaderType_Forward = 0,
	PixelShaderType_FullScreen,
	PixelShaderType_Count,
};

enum ConstantBufferType
{
	ConstantBufferType_Camera = 0,
	ConstantBufferType_Count,
};

class ShaderLib
{
public:
	ShaderLib(Renderer& renderer);

	void Initialize();
	void Uninitialize();

	std::shared_ptr<VertexShader> GetVertexShader(VetextShaderType shaderType);
	std::shared_ptr<InputLayout> GetVertexLayout(InputLayoutType layoutType);
	std::shared_ptr<PixelShader> GetPixelShader(PixelShaderType shaderType);
	std::shared_ptr<GPUBuffer> GetConstantBuffer(ConstantBufferType bufferType);

private:
	void LoadShader();
	void LoadVertexShaders();
	void LoadPixelShaders();
	void LoadBuffers();

private:
	Renderer & mRenderer;

	// TODO replacement
	std::shared_ptr<VertexShader> mVertexShader[VertexShaderType_Count];
	std::shared_ptr<InputLayout> mInputLayout[InputLayoutType_Count];
	std::shared_ptr<PixelShader> mPixelShader[PixelShaderType_Count];

	std::shared_ptr<GPUBuffer> mConstantBuffer[ConstantBufferType_Count];
};

class CustomShaderLib : public ShaderLib
{
public:
	CustomShaderLib();

	template<typename ShaderType>
	void RegisterShader();
};

}