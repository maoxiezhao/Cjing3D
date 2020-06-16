#include "terrainPass.h"
#include "system\sceneSystem.h"
#include "resource\resourceManager.h"
#include "renderer\stateManager.h"
#include "renderer\RHI\rhiFactory.h"

namespace Cjing3D{

// 开启地形测试数据
//#define _TERRAIN_DEBUG_

// simple terrain rendering
// 目前地形系统先简单采用的基于QuadTree的LOD和高度图的方法绘制地形

namespace
{
	std::shared_ptr<Shader> mTerrainVS = nullptr;
	std::shared_ptr<InputLayout> mTerrainIL = nullptr;
	std::shared_ptr<Shader> mTerrainPS = nullptr;
	std::shared_ptr<Shader> mTerrainHS = nullptr;
	std::shared_ptr<Shader> mTerrainDS = nullptr;

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
	for (auto& kvp : mTerrainTreeMap) {
		if (kvp.second != nullptr) {
			kvp.second->Uninitialize();
		}
	}
	mTerrainTreeMap.clear();
	mToRemovedTerrainTree.clear();

	terrainPSO.Clear();

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

	PipelineStateDesc desc = {};
	desc.mInputLayout = mTerrainIL;
	desc.mVertexShader = mTerrainVS;
	desc.mPixelShader = mTerrainPS;
	desc.mHullShader = mTerrainHS;
	desc.mDomainShader = mTerrainDS;
	desc.mPrimitiveTopology = PATCHLIST_4;
	desc.mBlendState = stateManager.GetBlendState(BlendStateID_Opaque);
	desc.mDepthStencilState = stateManager.GetDepthStencilState(DepthStencilStateID_GreaterEqualReadWrite);
	desc.mRasterizerState = stateManager.GetRasterizerState(RasterizerStateID_Front);

	mRenderer.GetDevice().CreatePipelineState(desc, terrainPSO);

#ifdef _TERRAIN_DEBUG_
	// TEST
	InitializeTestData();
#endif
}

void TerrainPass::InitializeTestData()
{
#ifdef _TERRAIN_DEBUG_
	mTerrainTree.Initialize(1024, 1024);
	mTerrainTree.SetTerrainPSO(terrainPSO);
	mTerrainTree.LoadHeightMap("Textures/HeightMap.png");
	
	ResourceManager& resourceManager = GlobalGetSubSystem<ResourceManager>();
	TerrainMaterial material;
	material.weightTexture  = resourceManager.GetOrCreate<RhiTexture2D>("Textures/TerrainWeights.dds", FORMAT_R8G8B8A8_UNORM, 1);
	material.detailTexture1 = resourceManager.GetOrCreate<RhiTexture2D>("Textures/TerrainDetail1.dds", FORMAT_R8G8B8A8_UNORM, 1);
	material.detailTexture2 = resourceManager.GetOrCreate<RhiTexture2D>("Textures/TerrainDetail2.dds", FORMAT_R8G8B8A8_UNORM, 1);
	material.detailTexture3 = resourceManager.GetOrCreate<RhiTexture2D>("Textures/TerrainDetail3.dds", FORMAT_R8G8B8A8_UNORM, 1);
	mTerrainTree.LoadTerrainMaterial(material);
#endif
}

void TerrainPass::InitializeShader()
{
	VertexLayoutDesc shadowLayout[] =
	{
		{ "POSITION_NORMAL_SUBSETINDEX", 0u, MeshComponent::VertexPosNormalSubset::format, 0u, APPEND_ALIGNED_ELEMENT,  INPUT_PER_VERTEX_DATA , 0u },

		// instance
		{ "INSTANCELOCALTRANS",  0u, FORMAT_R32G32B32A32_FLOAT, 1u, APPEND_ALIGNED_ELEMENT, INPUT_PER_INSTANCE_DATA, 1u },
		{ "INSTANCETERRAIN",0u, FORMAT_R32G32B32A32_UINT,  1u, APPEND_ALIGNED_ELEMENT, INPUT_PER_INSTANCE_DATA, 1u }
	};

	ResourceManager& resourceManager = GlobalGetSubSystem<ResourceManager>();
	const std::string shaderPath = resourceManager.GetStandardResourceDirectory(Resource_Shader);

	auto vsinfo = mRenderer.LoadVertexShaderInfo(shaderPath + "terrainVS.cso", shadowLayout, ARRAYSIZE(shadowLayout));
	mTerrainVS = vsinfo.mVertexShader;
	mTerrainIL = vsinfo.mInputLayout;
	mTerrainPS = mRenderer.LoadShader(SHADERSTAGES_PS, shaderPath + "terrainPS.cso");
	mTerrainHS = mRenderer.LoadShader(SHADERSTAGES_HS, shaderPath + "terrainHS.cso");
	mTerrainDS = mRenderer.LoadShader(SHADERSTAGES_DS, shaderPath + "terrainDS.cso");
}

void TerrainPass::UpdatePerFrameData(F32 deltaTime)
{
	for (auto& kvp : mTerrainTreeMap) {
		if (kvp.second != nullptr) {
			kvp.second->UpdatePerFrameData(deltaTime);
		}
	}

	// refresh current terrain map
	Renderer& renderer = GlobalGetSubSystem<Renderer>();
	Scene& scene = renderer.GetMainScene();
	for (auto& kvp : mTerrainTreeMap) 
	{
		bool needRemove = false;
		ECS::Entity entity = kvp.first;
		if (!scene.mTerrains.Contains(entity)) {
			needRemove = true;
		}

		if (needRemove) {
			mToRemovedTerrainTree.push_back(entity);
		}
	}

	// process removed terrainTree
	for (auto entity : mToRemovedTerrainTree)
	{
		auto it = mTerrainTreeMap.find(entity);
		if (it != mTerrainTreeMap.end() && it->second != nullptr) {
			it->second->Uninitialize();
		}
		mTerrainTreeMap.erase(it);
	}
	mToRemovedTerrainTree.clear();
}

void TerrainPass::RefreshRenderData()
{
	Renderer& renderer = GlobalGetSubSystem<Renderer>();
	Scene& scene = renderer.GetMainScene();

	for (auto& kvp : mTerrainTreeMap) {
		if (kvp.second != nullptr) {
			ECS::Entity entity = kvp.first;
			const auto transform = scene.GetComponent<TransformComponent>(entity);
			if (transform == nullptr) {
				continue;
			}

			kvp.second->RefreshRenderData(*transform);
		}
	}
}

void TerrainPass::Render()
{

	GraphicsDevice& device = mRenderer.GetDevice();
	device.BeginEvent("RenderTerrains");

	for (auto& kvp : mTerrainTreeMap) {
		if (kvp.second != nullptr) {
			kvp.second->Render();
		}
	}

	device.EndEvent();
}

U32 TerrainPass::GetElevation(ECS::Entity entity) const
{
	auto it = mTerrainTreeMap.find(entity);
	if (it != mTerrainTreeMap.end()) {
		return it->second->GetElevation();
	}
	return 0;
}

void TerrainPass::SetElevation(ECS::Entity entity, U32 elevation)
{
	auto it = mTerrainTreeMap.find(entity);
	if (it != mTerrainTreeMap.end()) {
		return it->second->SetElevation(elevation);
	}
}

TerrainTreePtr TerrainPass::RegisterTerrain(ECS::Entity entity, U32 width, U32 height, U32 elevation)
{
	auto it = mTerrainTreeMap.find(entity);
	if (it != mTerrainTreeMap.end())
	{
		// erase removed queue
		auto vIt = std::find(mToRemovedTerrainTree.begin(), mToRemovedTerrainTree.end(), entity);
		if (vIt != mToRemovedTerrainTree.end()) {
			mToRemovedTerrainTree.erase(vIt);
		}
		return nullptr;
	}

	TerrainTreePtr newTerrainTree = std::make_shared<TerrainTree>();
	newTerrainTree->Initialize(width, height);
	newTerrainTree->SetElevation(elevation);
	newTerrainTree->SetTerrainPSO(terrainPSO);

	mTerrainTreeMap[entity] = newTerrainTree;
	return newTerrainTree;
}

void TerrainPass::UnRegisterTerrain(ECS::Entity entity)
{
	auto it = mTerrainTreeMap.find(entity);
	if (it != mTerrainTreeMap.end())
	{
		auto vIt = std::find(mToRemovedTerrainTree.begin(), mToRemovedTerrainTree.end(), entity);
		if (vIt != mToRemovedTerrainTree.end()) {
			return;
		}

		mToRemovedTerrainTree.push_back(entity);
	}
}

TerrainTreePtr TerrainPass::GetTerrainTree(ECS::Entity entity)
{
	auto it = mTerrainTreeMap.find(entity);
	if (it != mTerrainTreeMap.end()) {
		return it->second;
	}

	return nullptr;
}



}