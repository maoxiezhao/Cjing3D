#include "renderer\preset\renderPreset.h"
#include "renderer\shaderLib.h"
#include "resource\resourceManager.h"
#include "system\component\mesh.h"

namespace Cjing3D {
namespace {
	
	ShaderPtr LoadShader(SHADERSTAGES stage, const std::string& path)
	{
		ShaderLib& shaderLib = Renderer::GetShaderLib();
		return shaderLib.LoadShader(stage, path);
	}

	VertexShaderInfo LoadVertexShaderInfo(const std::string& path, VertexLayoutDesc* desc, U32 numElements)
	{
		ShaderLib& shaderLib = Renderer::GetShaderLib();
		return shaderLib.LoadVertexShaderInfo(path, desc, numElements);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void LoadVertexShaders(std::shared_ptr<InputLayout>* inputLayouts, std::shared_ptr<Shader>* shaders)
	{
		Logger::Info("LoadVertexShaders");
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
			auto vsinfo = LoadVertexShaderInfo("objectVS.cso", layout, ARRAYSIZE(layout));
			shaders[VertexShaderType_ObjectAll] = vsinfo.mVertexShader;
			inputLayouts[InputLayoutType_ObjectAll] = vsinfo.mInputLayout;
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
			auto vsinfo = LoadVertexShaderInfo("objectPosTexVS.cso", posTexLayout, ARRAYSIZE(posTexLayout));
			shaders[VertexShaderType_ObjectPosTex] = vsinfo.mVertexShader;
			inputLayouts[InputLayoutType_ObjectPosTex] = vsinfo.mInputLayout;
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
			auto vsinfo = LoadVertexShaderInfo("objectPosVS.cso", posLayout, ARRAYSIZE(posLayout));
			shaders[VertexShaderType_ObjectPos] = vsinfo.mVertexShader;
			inputLayouts[InputLayoutType_ObjectPos] = vsinfo.mInputLayout;
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// full screen vs
		auto fullScreenVSInfo = LoadVertexShaderInfo("screenVS.cso", nullptr, 0);
		shaders[VertexShaderType_FullScreen] = fullScreenVSInfo.mVertexShader;

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// image vs
		auto imageVSInfo = LoadVertexShaderInfo("imageVS.cso", nullptr, 0);
		shaders[VertexShaderType_Image] = imageVSInfo.mVertexShader;

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
		auto shadowVSInfo = LoadVertexShaderInfo("shadowVS.cso", shadowLayout, ARRAYSIZE(shadowLayout));
		shaders[VertexShaderType_Shadow] = shadowVSInfo.mVertexShader;
		inputLayouts[InputLayoutType_Shadow] = shadowVSInfo.mInputLayout;

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// cube shadow vs
		if (!Renderer::GetDevice().CheckGraphicsFeatureSupport(GraphicsFeatureSupport::VIEWPORT_AND_RENDERTARGET_ARRAYINDEX_WITHOUT_GS)) {
			Debug::Warning("Failed to load cubeShadowVS: Feature not support: VIEWPORT_AND_RENDERTARGET_ARRAYINDEX_WITHOUT_GS");
		}
		else {
			shaders[VertexShaderType_ShadowCube] = LoadShader(SHADERSTAGES_VS, "cubeShadowVS.cso");
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// sky vs
		auto skyVSInfo = LoadVertexShaderInfo("skyVS.cso", nullptr, 0);
		shaders[VertexShaderType_Sky] = skyVSInfo.mVertexShader;

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// color pos
		VertexLayoutDesc colorPosLayout[] =
		{
			VertexLayoutDesc::VertexData("POSITION", 0u, FORMAT_R32G32B32A32_FLOAT, 0u),
			VertexLayoutDesc::VertexData("TEXCOORD", 0u, FORMAT_R32G32B32A32_FLOAT, 0u)
		};
		auto colorPosVSInfo = LoadVertexShaderInfo("posColorVS.cso", colorPosLayout, ARRAYSIZE(colorPosLayout));
		shaders[VertexShaderType_PosColor] = colorPosVSInfo.mVertexShader;
		inputLayouts[InputLayoutType_PosColor] = colorPosVSInfo.mInputLayout;
	}

	void LoadPixelShaders(std::shared_ptr<Shader>* shaders)
	{
		Logger::Info("LoadPixelShaders");
		// object
		shaders[PixelShaderType_Object_Forward]                  = LoadShader(SHADERSTAGES_PS, "objectForwardPS.cso");
		shaders[PixelShaderType_Object_Forward_Transparent]      = LoadShader(SHADERSTAGES_PS, "objectForwardTransparentPS.cso");
		shaders[PixelShaderType_Object_AlphaTest]				 = LoadShader(SHADERSTAGES_PS, "objectAlphaTestPS.cso");
		shaders[PixelShaderType_Object_TiledForward]			 = LoadShader(SHADERSTAGES_PS, "objectTiledForwardPS.cso");
		shaders[PixelShaderType_Object_TiledForward_Transparent] = LoadShader(SHADERSTAGES_PS, "objectTiledForwardTransparentPS.cso");
		shaders[PixelShaderType_FullScreen]						 = LoadShader(SHADERSTAGES_PS, "screenPS.cso");
		shaders[PixelShaderType_Image]							 = LoadShader(SHADERSTAGES_PS, "imagePS.cso");
		shaders[PixelShaderType_Sky]							 = LoadShader(SHADERSTAGES_PS, "skyPS.cso");
		shaders[PixelShaderType_PosColor]						 = LoadShader(SHADERSTAGES_PS, "posColorPS.cso");
	}

	void LoadComputeShaders(std::shared_ptr<Shader>* shaders)
	{
		Logger::Info("LoadComputeShaders");
		shaders[ComputeShaderType_Tonemapping]		       = LoadShader(SHADERSTAGES_CS, "toneMapping.cso");
		shaders[ComputeShaderType_MipmapGenerate]		   = LoadShader(SHADERSTAGES_CS, "mipmapGenerateCS.cso");
		shaders[ComputeShaderType_MipmapCubeGenerate]      = LoadShader(SHADERSTAGES_CS, "mipmapGenerateCubeCS.cso");
		shaders[ComputeShaderType_MipmapGenerateUnorm]     = LoadShader(SHADERSTAGES_CS, "mipmapGenerateUnormCS.cso");
		shaders[ComputeShaderType_MipmapCubeGenerateUnorm] = LoadShader(SHADERSTAGES_CS, "mipmapGenerateCubeUnormCS.cso");
		shaders[ComputeShaderType_FXAA]					   = LoadShader(SHADERSTAGES_CS, "fxaaCS.cso");
		shaders[ComputeShaderType_Skinning]				   = LoadShader(SHADERSTAGES_CS, "skinningCS.cso");
		shaders[ComputeShaderType_TiledFrustum]			   = LoadShader(SHADERSTAGES_CS, "tileFrustumCS.cso");
		shaders[ComputeShaderType_LightTiledCulling]       = LoadShader(SHADERSTAGES_CS, "lightTiledCullingCS.cso");
		shaders[ComputeShaderType_LinearDepth]			   = LoadShader(SHADERSTAGES_CS, "linearDepthCS.cso");
		shaders[ComputeShaderType_SSAO]				       = LoadShader(SHADERSTAGES_CS, "ssaoCS.cso");
		shaders[ComputeShaderType_GaussianBlur]			   = LoadShader(SHADERSTAGES_CS, "gaussianBlurCS.cso");
		shaders[ComputeShaderType_BilateralBlur]		   = LoadShader(SHADERSTAGES_CS, "bilateralBlurCS.cso");
		shaders[ComputeShaderType_BilateralUpsample]	   = LoadShader(SHADERSTAGES_CS, "bilateralUpsampleCS.cso");
	}
}

void RenderPreset::LoadShaderPreset()
{
	LoadVertexShaders(mInputLayout, mVertexShader);
	LoadPixelShaders(mPixelShader);
	LoadComputeShaders(mComputeShader);
}
}