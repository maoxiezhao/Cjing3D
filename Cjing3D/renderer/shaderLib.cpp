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
	LoadBuffers();
	LoadShader();
}

void ShaderLib::Uninitialize()
{
}

void ShaderLib::LoadShader()
{
	LoadVertexShaders();
	LoadPixelShaders();
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
	}
}

void ShaderLib::LoadBuffers()
{
	// init all constant buffer
	auto& device = mRenderer.GetDevice();
	for (size_t i = 0; i < ConstantBufferType_Count; i++) {
		mConstantBuffer[i] = std::make_shared<GPUBuffer>();
	}

	// TODO create by rhiFactory
	// Camera buffer
	GPUBufferDesc desc = {};
	desc.mUsage = USAGE_DYNAMIC;	// camera buffer ¸üÐÂÆµ·±
	desc.mCPUAccessFlags = CPU_ACCESS_WRITE;
	desc.mBindFlags = BIND_CONSTANT_BUFFER;
	desc.mByteWidth = sizeof(CameraCB);

	const auto result = device.CreateBuffer(&desc, *mConstantBuffer[ConstantBufferType_Camera], nullptr);
	Debug::ThrowIfFailed(result, "failed to create camera constant buffer:%08x", result);
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

std::shared_ptr<GPUBuffer> ShaderLib::GetConstantBuffer(ConstantBufferType bufferType)
{
	Debug::CheckAssertion(bufferType < ConstantBufferType::ConstantBufferType_Count);
	return mConstantBuffer[static_cast<U32>(bufferType)];
}

}
