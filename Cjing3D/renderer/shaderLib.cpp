#include "shaderLib.h"
#include "renderer.h"
#include "resource\resourceManager.h"
#include "renderer\RHI\rhiShader.h"
#include "system\sceneSystem.h"

namespace Cjing3D
{

ShaderLib::ShaderLib(Renderer & renderer) :
	mRenderer(renderer)
{
}

void ShaderLib::Initialize()
{
	LoadVertexShaders();
	LoadPixelShaders();
	LoadComputeShaders();
}

void ShaderLib::Uninitialize()
{
	for (int i = 0; i < VertexShaderType_Count; i++) {
		mVertexShader[i] = nullptr;
	}
	for (int i = 0; i < PixelShaderType_Count; i++) {
		mPixelShader[i] = nullptr;
	}
	for (int i = 0; i < ComputeShaderType_Count; i++) {
		mComputeShader[i] = nullptr;
	}
}

void ShaderLib::LoadVertexShaders()
{
	auto& resourceManager = mRenderer.GetResourceManager();
	const std::string shaderPath = resourceManager.GetStandardResourceDirectory(Resrouce_VertexShader);
	{
		VertexLayoutDesc layout[] = 
		{
			{ "POSITION_NORMAL_SUBSETINDEX", 0u, MeshComponent::VertexPosNormalSubset::format, 0u, APPEND_ALIGNED_ELEMENT,  INPUT_PER_VERTEX_DATA , 0u },
			{ "TEXCOORD", 0u, MeshComponent::VertexTex::format, 1u, APPEND_ALIGNED_ELEMENT,  INPUT_PER_VERTEX_DATA , 0u },

			// instance
			{ "MAIT", 0u, FORMAT_R32G32B32A32_FLOAT, 2u, APPEND_ALIGNED_ELEMENT,  INPUT_PER_INSTANCE_DATA , 1u },
			{ "MAIT", 1u, FORMAT_R32G32B32A32_FLOAT, 2u, APPEND_ALIGNED_ELEMENT,  INPUT_PER_INSTANCE_DATA , 1u },
			{ "MAIT", 2u, FORMAT_R32G32B32A32_FLOAT, 2u, APPEND_ALIGNED_ELEMENT,  INPUT_PER_INSTANCE_DATA , 1u },
			{ "COLOR",0u, FORMAT_R32G32B32A32_FLOAT, 2u, APPEND_ALIGNED_ELEMENT,  INPUT_PER_INSTANCE_DATA , 1u }
		};
		auto vsinfo = resourceManager.GetOrCreate<VertexShaderInfo>(shaderPath + "objectVS.cso", layout, 6);
		mVertexShader[VertexShaderType_Transform] = vsinfo->mVertexShader;
		mInputLayout[InputLayoutType_Transform] = vsinfo->mInputLayout;
	
		// full screen vs
		auto fullScreenVSInfo = resourceManager.GetOrCreate<VertexShaderInfo>(shaderPath + "screenVS.cso", nullptr, 0);
		mVertexShader[VertexShaderType_FullScreen] = fullScreenVSInfo->mVertexShader;

		// image vs
		auto imageVSInfo = resourceManager.GetOrCreate<VertexShaderInfo>(shaderPath + "imageVS.cso", nullptr, 0);
		mVertexShader[VertexShaderType_Image] = imageVSInfo->mVertexShader;
	}
}

void ShaderLib::LoadPixelShaders()
{
	auto& resourceManager = mRenderer.GetResourceManager();
	const std::string shaderPath = resourceManager.GetStandardResourceDirectory(Resrouce_PixelShader);
	{
		mPixelShader[PixelShaderType_Forward] = resourceManager.GetOrCreate<PixelShader>(shaderPath + "objectForwardPS.cso");

		// full screen ps
		mPixelShader[PixelShaderType_FullScreen] = resourceManager.GetOrCreate<PixelShader>(shaderPath + "screenPS.cso");

		// image ps
		mPixelShader[PixelShaderType_Image] = resourceManager.GetOrCreate<PixelShader>(shaderPath + "imagePS.cso");
	}
}

void ShaderLib::LoadComputeShaders()
{
	auto& resourceManager = mRenderer.GetResourceManager();
	const std::string shaderPath = resourceManager.GetStandardResourceDirectory(Resource_ComputeShader);
	{
		mComputeShader[ComputeShaderType_Tonemapping] = resourceManager.GetOrCreate<ComputeShader>(shaderPath + "toneMapping.cso");
	}
}

std::shared_ptr<VertexShader> ShaderLib::GetVertexShader(VetextShaderType shaderType)
{
	Debug::CheckAssertion(shaderType < VetextShaderType::VertexShaderType_Count);
	return mVertexShader[static_cast<U32>(shaderType)];
}

std::shared_ptr<InputLayout> ShaderLib::GetVertexLayout(InputLayoutType layoutType)
{
	return mInputLayout[static_cast<U32>(layoutType)];
}

std::shared_ptr<PixelShader> ShaderLib::GetPixelShader(PixelShaderType shaderType)
{
	Debug::CheckAssertion(shaderType < PixelShaderType::PixelShaderType_Count);
	return mPixelShader[static_cast<U32>(shaderType)];
}

std::shared_ptr<ComputeShader> ShaderLib::GetComputeShader(ComputeShaderType shaderType)
{
	Debug::CheckAssertion(shaderType < ComputeShaderType::ComputeShaderType_Count);
	return mComputeShader[static_cast<U32>(shaderType)];
}

}
