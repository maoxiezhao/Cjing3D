#include "shaderLib.h"
#include "renderer.h"
#include "resource\resourceManager.h"
#include "renderer\RHI\rhiShader.h"
#include "system\sceneSystem.h"

namespace Cjing3D
{

ShaderLib::ShaderLib()
{
}

void ShaderLib::Initialize()
{
}

void ShaderLib::Uninitialize()
{
}

InputLayoutPtr ShaderLib::LoadInputLayout(ShaderPtr Shader, VertexLayoutDesc* desc, U32 numElements)
{
	InputLayoutPtr inputLayoutPtr = CJING_MAKE_SHARED<InputLayout>();
	if (Shader != nullptr && desc != nullptr)
	{
		const HRESULT result = Renderer::GetDevice().CreateInputLayout(desc, numElements, *Shader, *inputLayoutPtr);
		Debug::ThrowIfFailed(result, "Failed to create input layout: %08X", result);
	}
	return inputLayoutPtr;
}

ShaderPtr ShaderLib::LoadShader(SHADERSTAGES stages, const std::string& path)
{
	// 目前所有shader都只是缓存了一份，但并未做索引，
	std::shared_ptr<Shader> shaderPtr = nullptr;
	if (!FileData::IsFileExists(path))
	{
		Debug::Warning("Failed to load shader:" + path);
		return shaderPtr;
	}

	size_t length = 0;
	const char* byteData = FileData::ReadFileBytes(path, length);
	if (byteData == nullptr)
	{
		Debug::Warning("Failed to load shader:" + path);
		return shaderPtr;
	}

	ShaderPtr ret = LoadShader(stages, byteData, length);
	SAFE_DELETE_ARRAY(byteData);

	return ret;
}

ShaderPtr ShaderLib::LoadShader(SHADERSTAGES stages, const void*data, size_t length)
{ 
	ShaderPtr shaderPtr = CJING_MAKE_SHARED<Shader>();
	{
		const HRESULT result = Renderer::GetDevice().CreateShader(stages, data, length, *shaderPtr);
		Debug::ThrowIfFailed(result, "Failed to create vertex shader: %08X", result);
	}
	return shaderPtr;
}

VertexShaderInfo ShaderLib::LoadVertexShaderInfo(const std::string& path, VertexLayoutDesc* desc, U32 numElements)
{
	VertexShaderInfo vertexShaderInfo = {};
	if (!FileData::IsFileExists(path))
	{
		Debug::Warning("Failed to load vertex shader:" + path);
		return vertexShaderInfo;
	}

	size_t length = 0;
	const char* byteData = FileData::ReadFileBytes(path, length);
	if (byteData == nullptr)
	{
		Debug::Warning("Failed to load vertex shader:" + path);
		return vertexShaderInfo;
	}

	vertexShaderInfo.mVertexShader = LoadShader(SHADERSTAGES_VS, byteData, length);
	vertexShaderInfo.mInputLayout = LoadInputLayout(vertexShaderInfo.mVertexShader, desc, numElements);

	SAFE_DELETE_ARRAY(byteData);

	return vertexShaderInfo;
}

}
