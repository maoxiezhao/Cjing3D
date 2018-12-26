#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\renderer.h"
#include "renderer\RHI\rhiResource.h"
#include "renderer\RHI\rhiShader.h"

namespace Cjing3D
{

enum VetextShaderType
{
	VertexShaderType_Transform = 0,
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
	PixelShaderType_Count,
};

class ShaderLib
{
public:
	ShaderLib(Renderer& renderer);

	void Initialize();
	void Uninitialize();

	std::shared_ptr<VertexShader> GetVertexShader(VetextShaderType shaderType);
	std::shared_ptr<PixelShader> GetPixelShader(PixelShaderType shaderType);

	ShaderInfoState GetShaderInfoState(Material& material);
private:
	void LoadShader();
	void LoadVertexShaders();
	void LoadPixelShaders();
	void LoadBuffers();

private:
	Renderer & mRenderer;

	// TODO replacement
	std::shared_ptr<VertexShader> mVertexShader[VertexShaderType_Count];
	std::shared_ptr<InputLayout> mInputOuts[InputLayoutType_Count];

	std::shared_ptr<PixelShader> mPixelShader[PixelShaderType_Count];

};

class CustomShaderLib : public ShaderLib
{
public:
	CustomShaderLib();

	template<typename ShaderType>
	void RegisterShader();
};

}