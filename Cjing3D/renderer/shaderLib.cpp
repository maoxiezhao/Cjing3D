#include "shaderLib.h"
#include "renderer.h"
#include "resource\resourceManager.h"
#include "renderer\RHI\rhiShader.h"
#include "core\sceneSystem.h"

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
	LoadShaderInfoState();
}

void ShaderLib::LoadVertexShaders()
{
	auto& resourceManager = mRenderer.GetResourceManager();
	const std::string shaderPath = resourceManager.GetStandardResourceDirectory(Resrouce_VertexShader);
	{
		VertexLayoutDesc layout[] = 
		{
			{ "POSITION_NORMAL_SUBSETINDEX", 0u, MeshComponent::VertexPosNormalSubset::format, 0u, APPEND_ALIGNED_ELEMENT,  INPUT_PER_VERTEX_DATA , 0u },
			{ "TEXCOORD", 0u, MeshComponent::VertexTex::format, 0u, APPEND_ALIGNED_ELEMENT,  INPUT_PER_VERTEX_DATA , 0u },

			// instance
			{ "MAIT", 0u, FORMAT_R32G32B32A32_FLOAT, 4u, APPEND_ALIGNED_ELEMENT,  INPUT_PER_VERTEX_DATA , 1u },
			{ "MAIT", 1u, FORMAT_R32G32B32A32_FLOAT, 4u, APPEND_ALIGNED_ELEMENT,  INPUT_PER_VERTEX_DATA , 1u },
			{ "MAIT", 2u, FORMAT_R32G32B32A32_FLOAT, 4u, APPEND_ALIGNED_ELEMENT,  INPUT_PER_VERTEX_DATA , 1u },
			{ "COLOR",0u, FORMAT_R32G32B32A32_FLOAT, 4u, APPEND_ALIGNED_ELEMENT,  INPUT_PER_VERTEX_DATA , 1u }
		};
		auto vsinfo = resourceManager.GetOrCreate<VertexShaderInfo>(shaderPath + "objectVS.cso", layout, 6);
	}
}

void ShaderLib::LoadPixelShaders()
{
}

void ShaderLib::LoadBuffers()
{
	// init all constant buffer
	auto& device = mRenderer.GetDevice();
	for (size_t i = 0; i < ConstantBufferType_Count; i++) {
		mConstantBuffer[i] = std::make_shared<GPUBuffer>(device);
	}

	// Camera buffer
	GPUBufferDesc desc = {};
	desc.mUsage = USAGE_DYNAMIC;	// camera buffer 更新频繁
	desc.mCPUAccessFlags = CPU_ACCESS_WRITE;
	desc.mBindFlags = BIND_CONSTANT_BUFFER;
	desc.mByteWidth = sizeof(CameraCB);

	const auto result = device.CreateBuffer(&desc, *mConstantBuffer[ConstantBufferType_Camera], nullptr);
	Debug::ThrowIfFailed(result, "failed to create camera constant buffer:%08x", result);
}

void ShaderLib::LoadShaderInfoState()
{
	// TEMP: 在getShaderInfoState时直接创建
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

ShaderInfoState ShaderLib::GetShaderInfoState(ShaderType shaderType, MaterialComponent & material)
{
	ShaderInfoState infoState;
	infoState.SetGraphicsDevice(&mRenderer.GetDevice());

	if (shaderType == ShaderType_Forward)
	{
		infoState.mVertexShader = GetVertexShader(VertexShaderType_Transform);
		infoState.mInputLayout = GetVertexLayout(InputLayoutType_Transform);
		infoState.mPixelShader = GetPixelShader(PixelShaderType_Forward);
	}

	return infoState;
}

}
