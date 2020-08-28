#include "shaderLib.h"
#include "renderer.h"
#include "resource\resourceManager.h"
#include "renderer\RHI\rhiShader.h"
#include "system\sceneSystem.h"

namespace Cjing3D {
namespace {
	std::map<std::string, ShaderPtr> mLoadedShaderMap;
	std::map<std::string, InputLayoutPtr> mLoadedInputLayoutMap;

	//////////////////////////////////////////////////////////////////////////////////////////////////////

	ShaderPtr GetOrCreateShader(const std::string& path)
	{
		auto it = mLoadedShaderMap.find(path);
		if (it != mLoadedShaderMap.end()) 
		{
			return it->second;
		}
		else
		{
			ShaderPtr shaderPtr = CJING_MAKE_SHARED<Shader>();
			mLoadedShaderMap[path] = shaderPtr;
			return shaderPtr;
		}
	}

	InputLayoutPtr GetOrCreateInputLayout(const std::string& path)
	{
		auto it = mLoadedInputLayoutMap.find(path);
		if (it != mLoadedInputLayoutMap.end())
		{
			return it->second;
		}
		else
		{
			InputLayoutPtr inputLayoutPtr = CJING_MAKE_SHARED<InputLayout>();
			mLoadedInputLayoutMap[path] = inputLayoutPtr;
			return inputLayoutPtr;
		}
	}

	void LoadShaderImpl(SHADERSTAGES stages, const void* data, size_t length, ShaderPtr& shaderPtr)
	{
		const HRESULT result = Renderer::GetDevice().CreateShader(stages, data, length, *shaderPtr);
		Debug::ThrowIfFailed(result, "Failed to create vertex shader: %08X", result);
	}

	void LoadInputLayout(ShaderPtr Shader, VertexLayoutDesc* desc, U32 numElements, InputLayoutPtr& inputLayoutPtr)
	{
		const HRESULT result = Renderer::GetDevice().CreateInputLayout(desc, numElements, *Shader, *inputLayoutPtr);
		Debug::ThrowIfFailed(result, "Failed to create input layout: %08X", result);
	}
}

ShaderLib::ShaderLib()
{
}

void ShaderLib::Initialize()
{
}

void ShaderLib::Uninitialize()
{
	for (auto kvp : mLoadedShaderMap) {
		kvp.second->Clear();
	}
	mLoadedShaderMap.clear();

	for (auto kvp : mLoadedInputLayoutMap) {
		kvp.second->Clear();
	}
	mLoadedInputLayoutMap.clear();
}

void ShaderLib::Reload()
{
	for (auto kvp : mLoadedShaderMap)
	{
		if (kvp.second->mStage == SHADERSTAGES_VS)
		{
			auto it = mLoadedInputLayoutMap.find(kvp.first);
			if (it != mLoadedInputLayoutMap.end())
			{
				InputLayoutPtr inputLayout = it->second;
				LoadVertexShaderInfo(kvp.first, inputLayout->mDescs.data(), inputLayout->mDescs.size());
				continue;
			}
		}

		LoadShader(kvp.second->mStage, kvp.first);
	}
}


ShaderPtr ShaderLib::LoadShader(SHADERSTAGES stages, const std::string& path)
{
	// 目前所有shader都只是缓存了一份，但并未做索引，
	auto resourceManager = GetGlobalContext().gResourceManager;
	auto shaderDirectory = resourceManager->GetStandardResourceDirectory(Resource_Shader);

	const std::string fullPath = shaderDirectory + path;
	std::shared_ptr<Shader> shaderPtr = nullptr;
	if (!FileData::IsFileExists(fullPath))
	{
		Debug::Warning("Failed to load shader:" + fullPath);
		return shaderPtr;
	}

	size_t length = 0;
	const char* byteData = FileData::ReadFileBytes(fullPath, length);
	if (byteData == nullptr)
	{
		Debug::Warning("Failed to load shader:" + fullPath);
		return shaderPtr;
	}

	shaderPtr = GetOrCreateShader(path);
	LoadShaderImpl(stages, byteData, length, shaderPtr);
	SAFE_DELETE_ARRAY(byteData);

	return shaderPtr;
}


VertexShaderInfo ShaderLib::LoadVertexShaderInfo(const std::string& path, VertexLayoutDesc* desc, U32 numElements)
{
	auto resourceManager = GetGlobalContext().gResourceManager;
	auto shaderDirectory = resourceManager->GetStandardResourceDirectory(Resource_Shader);

	const std::string fullPath = shaderDirectory + path;
	VertexShaderInfo vertexShaderInfo = {};
	if (!FileData::IsFileExists(fullPath))
	{
		Debug::Warning("Failed to load vertex shader:" + fullPath);
		return vertexShaderInfo;
	}

	size_t length = 0;
	const char* byteData = FileData::ReadFileBytes(fullPath, length);
	if (byteData == nullptr)
	{
		Debug::Warning("Failed to load vertex shader:" + fullPath);
		return vertexShaderInfo;
	}

	vertexShaderInfo.mVertexShader = GetOrCreateShader(path);
	vertexShaderInfo.mInputLayout = GetOrCreateInputLayout(path);

	LoadShaderImpl(SHADERSTAGES_VS, byteData, length, vertexShaderInfo.mVertexShader);
	LoadInputLayout(vertexShaderInfo.mVertexShader, desc, numElements, vertexShaderInfo.mInputLayout);
	SAFE_DELETE_ARRAY(byteData);

	return vertexShaderInfo;
}
}
