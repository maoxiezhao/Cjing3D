#include "terrainPass.h"
#include "system\sceneSystem.h"
#include "resource\resourceManager.h"
#include "renderer\stateManager.h"
#include "renderer\RHI\rhiFactory.h"

namespace Cjing3D{

// 开启地形测试数据
#define _TERRAIN_DEBUG_

// simple terrain rendering
// 目前地形系统先简单采用的基于QuadTree的LOD和高度图的方法绘制地形

namespace
{
	std::shared_ptr<VertexShader> mTerrainVS = nullptr;
	std::shared_ptr<InputLayout> mTerrainIL = nullptr;
	std::shared_ptr<PixelShader> mTerrainPS = nullptr;
	std::shared_ptr<HullShader> mTerrainHS = nullptr;
	std::shared_ptr<DomainShader> mTerrainDS = nullptr;

	PipelineState terrainPSO;
}

TerrainPass::TerrainPass(Renderer& renderer) :
	RenderPass(renderer)
{
}

TerrainPass::~TerrainPass()
{

}

void TerrainPass::Uninitialize()
{
	terrainPSO.Clear();
	mTerrainTree.Uninitialize();

	mTerrainVS = nullptr;
	mTerrainIL = nullptr;
	mTerrainPS = nullptr;
	mTerrainHS = nullptr;
	mTerrainDS = nullptr;
}


void TerrainPass::Initialize()
{
	InitializeShader();

	// initialize pso
	ShaderLib& shaderLib = mRenderer.GetShaderLib();
	StateManager& stateManager = mRenderer.GetStateManager();

	terrainPSO.Register(&mRenderer.GetDevice());
	terrainPSO.mInputLayout = mTerrainIL;
	terrainPSO.mVertexShader = mTerrainVS;
	terrainPSO.mPixelShader = mTerrainPS;
	terrainPSO.mHullShader = mTerrainHS;
	terrainPSO.mDomainShader = mTerrainDS;
	terrainPSO.mPrimitiveTopology = PATCHLIST_4;
	terrainPSO.mBlendState = stateManager.GetBlendState(BlendStateID_Opaque);
	terrainPSO.mDepthStencilState = stateManager.GetDepthStencilState(DepthStencilStateID_GreaterEqualReadWrite);
	terrainPSO.mRasterizerState = stateManager.GetRasterizerState(RasterizerStateID_Front);

	mTerrainTree.Initialize(1024, 1024);

#ifdef _TERRAIN_DEBUG_
	// TEST
	InitializeTestData();
#endif
}

void TerrainPass::InitializeTestData()
{
	mTerrainTree.SetTerrainPSO(terrainPSO);
	mTerrainTree.LoadHeightMap("Textures/HeightMap.png");
	
	ResourceManager& resourceManager = GlobalGetSubSystem<ResourceManager>();
	TerrainMaterial material;
	material.weightTexture  = resourceManager.GetOrCreate<RhiTexture2D>("Textures/TerrainWeights.dds", FORMAT_R8G8B8A8_UNORM, 1);
	material.detailTexture1 = resourceManager.GetOrCreate<RhiTexture2D>("Textures/TerrainDetail1.dds", FORMAT_R8G8B8A8_UNORM, 1);
	material.detailTexture2 = resourceManager.GetOrCreate<RhiTexture2D>("Textures/TerrainDetail2.dds", FORMAT_R8G8B8A8_UNORM, 1);
	material.detailTexture3 = resourceManager.GetOrCreate<RhiTexture2D>("Textures/TerrainDetail3.dds", FORMAT_R8G8B8A8_UNORM, 1);
	mTerrainTree.LoadTerrainMaterial(material);
}

void TerrainPass::InitializeShader()
{
	VertexLayoutDesc shadowLayout[] =
	{
		{ "POSITION_NORMAL_SUBSETINDEX", 0u, MeshComponent::VertexPosNormalSubset::format, 0u, APPEND_ALIGNED_ELEMENT,  INPUT_PER_VERTEX_DATA , 0u },

		// instance
		{ "INSTANCEMAT", 0u, FORMAT_R32G32B32A32_FLOAT,  1u, APPEND_ALIGNED_ELEMENT,  INPUT_PER_INSTANCE_DATA , 1u },
		{ "INSTANCEMAT", 1u, FORMAT_R32G32B32A32_FLOAT,  1u, APPEND_ALIGNED_ELEMENT,  INPUT_PER_INSTANCE_DATA , 1u },
		{ "INSTANCEMAT", 2u, FORMAT_R32G32B32A32_FLOAT,  1u, APPEND_ALIGNED_ELEMENT,  INPUT_PER_INSTANCE_DATA , 1u },
		{ "INSTANCECOLOR",0u, FORMAT_R32G32B32A32_FLOAT, 1u, APPEND_ALIGNED_ELEMENT,  INPUT_PER_INSTANCE_DATA , 1u }
	};

	ResourceManager& resourceManager = GlobalGetSubSystem<ResourceManager>();
	const std::string shaderPath = resourceManager.GetStandardResourceDirectory(Resrouce_VertexShader);

	auto vsinfo = resourceManager.GetOrCreate<VertexShaderInfo>(shaderPath + "terrainVS.cso", shadowLayout, ARRAYSIZE(shadowLayout));
	mTerrainVS = vsinfo->mVertexShader;
	mTerrainIL = vsinfo->mInputLayout;
	mTerrainPS = resourceManager.GetOrCreate<PixelShader>(shaderPath + "terrainPS.cso");
	mTerrainHS = resourceManager.GetOrCreate<HullShader>(shaderPath + "terrainHS.cso");
	mTerrainDS = resourceManager.GetOrCreate<DomainShader>(shaderPath + "terrainDS.cso");
}

void TerrainPass::UpdatePerFrameData(F32 deltaTime)
{
	mTerrainTree.UpdatePerFrameData(deltaTime);
}

void TerrainPass::RefreshRenderData()
{
	mTerrainTree.RefreshRenderData();
}

void TerrainPass::Render()
{
	GraphicsDevice& device = mRenderer.GetDevice();
	device.BeginEvent("RenderTerrains");

	mTerrainTree.Render();

	device.EndEvent();
}

U32 TerrainPass::GetElevation() const
{
	return mTerrainTree.GetElevation();
}

void TerrainPass::SetElevation(U32 elevation)
{
	mTerrainTree.SetElevation(elevation);
}



}