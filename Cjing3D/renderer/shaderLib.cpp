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
	Logger::Info("LoadVertexShaders");
	auto& resourceManager = GlobalGetSubSystem<ResourceManager>();
	const std::string shaderPath = resourceManager.GetStandardResourceDirectory(Resource_Shader);
	{
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// object all
		{
			VertexLayoutDesc layout[] =
			{
				// vertex
				VertexLayoutDesc::VertexData("POSITION_NORMAL_SUBSETINDEX", 0u, MeshComponent::VertexPosNormalSubset::format, 0u),
				VertexLayoutDesc::VertexData("TEXCOORD",				    0u, MeshComponent::VertexTex::format,			  1u),
				VertexLayoutDesc::VertexData("COLOR",						0u, MeshComponent::VertexColor::format,			  2u),

				// instance
				VertexLayoutDesc::InstanceData("INSTANCEMAT",     0u, FORMAT_R32G32B32A32_FLOAT, 3u),
				VertexLayoutDesc::InstanceData("INSTANCEMAT",     1u, FORMAT_R32G32B32A32_FLOAT, 3u),
				VertexLayoutDesc::InstanceData("INSTANCEMAT",     2u, FORMAT_R32G32B32A32_FLOAT, 3u),
				VertexLayoutDesc::InstanceData("INSTANCEUSERDATA",0u, FORMAT_R32G32B32A32_UINT,  3u),
			};
			auto vsinfo = LoadVertexShaderInfo(shaderPath + "objectVS.cso", layout, ARRAYSIZE(layout));
			mVertexShader[VertexShaderType_ObjectAll] = vsinfo.mVertexShader;
			mInputLayout[InputLayoutType_ObjectAll] = vsinfo.mInputLayout;
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// object pos tex
		{
			VertexLayoutDesc posTexLayout[] =
			{
				// vertex
				VertexLayoutDesc::VertexData("POSITION_NORMAL_SUBSETINDEX", 0u, MeshComponent::VertexPosNormalSubset::format, 0u),
				VertexLayoutDesc::VertexData("TEXCOORD",			        0u, MeshComponent::VertexTex::format,			  1u),

				// instance
				VertexLayoutDesc::InstanceData("INSTANCEMAT",     0u, FORMAT_R32G32B32A32_FLOAT, 2u),
				VertexLayoutDesc::InstanceData("INSTANCEMAT",     1u, FORMAT_R32G32B32A32_FLOAT, 2u),
				VertexLayoutDesc::InstanceData("INSTANCEMAT",     2u, FORMAT_R32G32B32A32_FLOAT, 2u),
				VertexLayoutDesc::InstanceData("INSTANCEUSERDATA",0u, FORMAT_R32G32B32A32_UINT,  2u),
			};
			auto vsinfo = LoadVertexShaderInfo(shaderPath + "objectPosTexVS.cso", posTexLayout, ARRAYSIZE(posTexLayout));
			mVertexShader[VertexShaderType_ObjectPosTex] = vsinfo.mVertexShader;
			mInputLayout[InputLayoutType_ObjectPosTex] = vsinfo.mInputLayout;
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// object pos
		{
			VertexLayoutDesc posLayout[] =
			{
				// vertex
				VertexLayoutDesc::VertexData("POSITION_NORMAL_SUBSETINDEX", 0u, MeshComponent::VertexPosNormalSubset::format, 0u),

				// instance
				VertexLayoutDesc::InstanceData("INSTANCEMAT",     0u, FORMAT_R32G32B32A32_FLOAT, 1u),
				VertexLayoutDesc::InstanceData("INSTANCEMAT",     1u, FORMAT_R32G32B32A32_FLOAT, 1u),
				VertexLayoutDesc::InstanceData("INSTANCEMAT",     2u, FORMAT_R32G32B32A32_FLOAT, 1u),
				VertexLayoutDesc::InstanceData("INSTANCEUSERDATA",0u, FORMAT_R32G32B32A32_UINT,  1u),
			};
			auto vsinfo = LoadVertexShaderInfo(shaderPath + "objectPosVS.cso", posLayout, ARRAYSIZE(posLayout));
			mVertexShader[VertexShaderType_ObjectPos] = vsinfo.mVertexShader;
			mInputLayout[InputLayoutType_ObjectPos] = vsinfo.mInputLayout;
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// full screen vs
		auto fullScreenVSInfo = LoadVertexShaderInfo(shaderPath + "screenVS.cso", nullptr, 0);
		mVertexShader[VertexShaderType_FullScreen] = fullScreenVSInfo.mVertexShader;

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// image vs
		auto imageVSInfo = LoadVertexShaderInfo(shaderPath + "imageVS.cso", nullptr, 0);
		mVertexShader[VertexShaderType_Image] = imageVSInfo.mVertexShader;

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// shadow vs
		VertexLayoutDesc shadowLayout[] =
		{
			// vertex
			VertexLayoutDesc::VertexData("POSITION_NORMAL_SUBSETINDEX", 0u, MeshComponent::VertexPosNormalSubset::format, 0u),

			// instance
			VertexLayoutDesc::InstanceData("INSTANCEMAT",     0u, FORMAT_R32G32B32A32_FLOAT, 1u),
			VertexLayoutDesc::InstanceData("INSTANCEMAT",     1u, FORMAT_R32G32B32A32_FLOAT, 1u),
			VertexLayoutDesc::InstanceData("INSTANCEMAT",     2u, FORMAT_R32G32B32A32_FLOAT, 1u),
			VertexLayoutDesc::InstanceData("INSTANCEUSERDATA",0u, FORMAT_R32G32B32A32_UINT,  1u),
		};
		auto shadowVSInfo = LoadVertexShaderInfo(shaderPath + "shadowVS.cso", shadowLayout, ARRAYSIZE(shadowLayout));
		mVertexShader[VertexShaderType_Shadow] = shadowVSInfo.mVertexShader;
		mInputLayout[InputLayoutType_Shadow] = shadowVSInfo.mInputLayout;

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// cube shadow vs
		if (!Renderer::GetDevice().CheckGraphicsFeatureSupport(GraphicsFeatureSupport::VIEWPORT_AND_RENDERTARGET_ARRAYINDEX_WITHOUT_GS)) {
			Debug::Warning("Failed to load cubeShadowVS: Feature not support: VIEWPORT_AND_RENDERTARGET_ARRAYINDEX_WITHOUT_GS");
		} else {
			mVertexShader[VertexShaderType_ShadowCube] = LoadShader(SHADERSTAGES_VS, shaderPath + "cubeShadowVS.cso");
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// sky vs
		auto skyVSInfo = LoadVertexShaderInfo(shaderPath + "skyVS.cso", nullptr, 0);
		mVertexShader[VertexShaderType_Sky] = skyVSInfo.mVertexShader;

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// color pos
		VertexLayoutDesc colorPosLayout[] =
		{
			VertexLayoutDesc::VertexData("POSITION", 0u, FORMAT_R32G32B32A32_FLOAT, 0u),
			VertexLayoutDesc::VertexData("TEXCOORD", 0u, FORMAT_R32G32B32A32_FLOAT, 0u)
		};
		auto colorPosVSInfo = LoadVertexShaderInfo(shaderPath + "posColorVS.cso", colorPosLayout, ARRAYSIZE(colorPosLayout));
		mVertexShader[VertexShaderType_PosColor] = colorPosVSInfo.mVertexShader;
		mInputLayout[InputLayoutType_PosColor] = colorPosVSInfo.mInputLayout;
	}
}

void ShaderLib::LoadPixelShaders()
{
	Logger::Info("LoadPixelShaders");
	auto& resourceManager = GlobalGetSubSystem<ResourceManager>();
	const std::string shaderPath = resourceManager.GetStandardResourceDirectory(Resource_Shader);
	{
		// object
		mPixelShader[PixelShaderType_Object_Forward] = LoadShader(SHADERSTAGES_PS, shaderPath + "objectForwardPS.cso");
		mPixelShader[PixelShaderType_Object_Forward_Transparent] = LoadShader(SHADERSTAGES_PS, shaderPath + "objectForwardTransparentPS.cso");
		mPixelShader[PixelShaderType_Object_AlphaTest] = LoadShader(SHADERSTAGES_PS, shaderPath + "objectAlphaTestPS.cso");
		mPixelShader[PixelShaderType_Object_TiledForward] = LoadShader(SHADERSTAGES_PS, shaderPath + "objectTiledForwardPS.cso");
		mPixelShader[PixelShaderType_Object_TiledForward_Transparent] = LoadShader(SHADERSTAGES_PS, shaderPath + "objectTiledForwardTransparentPS.cso");

		// full screen ps
		mPixelShader[PixelShaderType_FullScreen] = LoadShader(SHADERSTAGES_PS, shaderPath + "screenPS.cso");
		// image ps
		mPixelShader[PixelShaderType_Image] = LoadShader(SHADERSTAGES_PS, shaderPath + "imagePS.cso");
		// full screen ps
		mPixelShader[PixelShaderType_Sky] = LoadShader(SHADERSTAGES_PS, shaderPath + "skyPS.cso");
		// color pos
		mPixelShader[PixelShaderType_PosColor] = LoadShader(SHADERSTAGES_PS, shaderPath + "posColorPS.cso");
	}
}

void ShaderLib::LoadComputeShaders()
{
	Logger::Info("LoadComputeShaders");
	auto& resourceManager = GlobalGetSubSystem<ResourceManager>();
	const std::string shaderPath = resourceManager.GetStandardResourceDirectory(Resource_Shader);
	{
		mComputeShader[ComputeShaderType_Tonemapping]       = LoadShader(SHADERSTAGES_CS, shaderPath + "toneMapping.cso");
		mComputeShader[ComputeShaderType_MipmapGenerate]    = LoadShader(SHADERSTAGES_CS, shaderPath + "mipmapGenerate.cso");
		mComputeShader[ComputeShaderType_FXAA]              = LoadShader(SHADERSTAGES_CS, shaderPath + "fxaaCS.cso");
		mComputeShader[ComputeShaderType_Skinning]          = LoadShader(SHADERSTAGES_CS, shaderPath + "skinningCS.cso");
		mComputeShader[ComputeShaderType_TiledFrustum]      = LoadShader(SHADERSTAGES_CS, shaderPath + "tileFrustumCS.cso");
		mComputeShader[ComputeShaderType_LightTiledCulling] = LoadShader(SHADERSTAGES_CS, shaderPath + "lightTiledCullingCS.cso");
		mComputeShader[ComputeShaderType_LinearDepth]       = LoadShader(SHADERSTAGES_CS, shaderPath + "linearDepthCS.cso");
		mComputeShader[ComputeShaderType_SSAO]              = LoadShader(SHADERSTAGES_CS, shaderPath + "ssaoCS.cso");
		mComputeShader[ComputeShaderType_GaussianBlur]      = LoadShader(SHADERSTAGES_CS, shaderPath + "gaussianBlurCS.cso");
		mComputeShader[ComputeShaderType_BilateralBlur]		= LoadShader(SHADERSTAGES_CS, shaderPath + "bilateralBlurCS.cso");
		mComputeShader[ComputeShaderType_BilateralUpsample] = LoadShader(SHADERSTAGES_CS, shaderPath + "bilateralUpsampleCS.cso");
	}
}

std::shared_ptr<Shader> ShaderLib::GetVertexShader(VetextShaderType shaderType)
{
	Debug::CheckAssertion(shaderType < VetextShaderType::VertexShaderType_Count);
	return mVertexShader[static_cast<size_t>(shaderType)];
}

std::shared_ptr<InputLayout> ShaderLib::GetVertexLayout(InputLayoutType layoutType)
{
	return mInputLayout[static_cast<size_t>(layoutType)];
}

std::shared_ptr<Shader> ShaderLib::GetPixelShader(PixelShaderType shaderType)
{
	Debug::CheckAssertion(shaderType < PixelShaderType::PixelShaderType_Count);
	return mPixelShader[static_cast<size_t>(shaderType)];
}

std::shared_ptr<Shader> ShaderLib::GetComputeShader(ComputeShaderType shaderType)
{
	Debug::CheckAssertion(shaderType < ComputeShaderType::ComputeShaderType_Count);
	return mComputeShader[static_cast<size_t>(shaderType)];
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
