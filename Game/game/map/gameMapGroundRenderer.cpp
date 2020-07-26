#include "GameMapGroundRenderer.h"
#include "resource\resourceManager.h"
#include "renderer\preset\renderPreset.h"
#include "renderer\shaderLib.h"
#include "definitions\gameRenderCommon.h"

namespace CjingGame
{
	namespace {
		GPUBuffer mGroundTilesetBuffer;
	}

	GameMapGroundRenderer::GameMapGroundRenderer()
	{
	}

	GameMapGroundRenderer::~GameMapGroundRenderer()
	{
	}

	void GameMapGroundRenderer::Uninitilize()
	{
		Scene& scene = Scene::GetScene();
		if (mMeshEntity != INVALID_ENTITY) 
		{
			MeshComponent* mesh = scene.mMeshes.GetComponent(mMeshEntity);
			if (mesh != nullptr) {
				mesh->ClearRenderData();
			}

			scene.RemoveEntity(mMeshEntity);
		}
	}

	void GameMapGroundRenderer::SetCurrentTileset(GameMapGroundTileset* tileset)
	{
		if (tileset == mGameMapGroundTileset) {
			return;
		}
		mGameMapGroundTileset = tileset;

		if (tileset != nullptr) 
		{
			MaterialComponent& material = *Scene::GetScene().mMaterials.GetComponent(mMaterialEntity);
			material.mBaseColorMapName = mGameMapGroundTileset->GetBaseColorTilesetPath();
			material.mBaseColorMap = mGameMapGroundTileset->GetBaseColorTexture();

		}
	}

	void GameMapGroundRenderer::BindPreRender()
	{
		if (mGameMapGroundTileset != nullptr)
		{
			auto tilesetSize = mGameMapGroundTileset->GetTilesetSize();
			GroundTileSetCB cb;
			cb.gGroundTilesetWidth = tilesetSize[0];
			cb.gGroundTilesetHeight = tilesetSize[1];
			cb.gGroundTilesetInvWidth = 1.0f / (F32)cb.gGroundTilesetWidth;
			cb.gGroundTilesetInvHeight = 1.0f / (F32)cb.gGroundTilesetHeight;

			Renderer::GetDevice().UpdateBuffer(mGroundTilesetBuffer, &cb);
			Renderer::GetDevice().BindConstantBuffer(SHADERSTAGES_VS, mGroundTilesetBuffer, CB_GETSLOT_NAME(GroundTileSetCB));
		}
	}

	void GameMapGroundRenderer::Initialize()
	{
		InitializeRender();
		InitializeMaterial();
		InitializeMesh();
	}

	void GameMapGroundRenderer::InitializeRender()
	{
		// initialize pipeline state
		RenderPreset& renderPreset = Renderer::GetRenderPreset();
		ResourceManager& resourceManager = GlobalGetSubSystem<ResourceManager>();
		const std::string shaderPath = resourceManager.GetStandardResourceDirectory(Resource_Shader);

		PipelineStateDesc desc = {};
		desc.mInputLayout = renderPreset.GetVertexLayout(InputLayoutType_ObjectAll);
		desc.mVertexShader = Renderer::LoadShader(SHADERSTAGES_VS, shaderPath + "mapGroundVS.cso");
		desc.mPixelShader = renderPreset.GetPixelShader(PixelShaderType_Object_Forward);
		desc.mPrimitiveTopology = TRIANGLELIST;
		desc.mBlendState = renderPreset.GetBlendState(BlendStateID_Opaque);
		desc.mDepthStencilState = renderPreset.GetDepthStencilState(DepthStencilStateID_DepthReadEqual);
		desc.mRasterizerState = renderPreset.GetRasterizerState(RasterizerStateID_Front);
		Renderer::RegisterPipelineState(RenderPassType_TiledForward, "MapGroundPSO", desc);

		// initialize constant buffer
		const HRESULT result = CreateDynamicConstantBuffer(Renderer::GetDevice(), mGroundTilesetBuffer, sizeof(GroundTileSetCB));
		Debug::ThrowIfFailed(result, "failed to create font constant buffer:%08x", result);
		Renderer::GetDevice().SetResourceName(mGroundTilesetBuffer, "FontCB");
	}

	void GameMapGroundRenderer::InitializeMaterial()
	{
		Scene& scene = Scene::GetScene();

		// initialize mesh component
		mMaterialEntity = scene.CreateEntityMaterial("GroundMaterial");
		Scene::GetScene().mMaterials.GetComponent(mMaterialEntity)->SetCustomShader("MapGroundPSO");
	}

	void GameMapGroundRenderer::InitializeMesh()
	{
		Scene& scene = Scene::GetScene();

		// initialize mesh component
		mMeshEntity = scene.CreateEntityMesh("GroundMesh");
		MeshComponent& mesh = *scene.mMeshes.GetComponent(mMeshEntity);

		// position
		const size_t vertexCount = 4;
		mesh.mVertexPositions.resize(vertexCount);
		mesh.mVertexPositions[0] = F32x3(0.0f, 0.0f, 0.0f);
		mesh.mVertexPositions[1] = F32x3(1.0f, 0.0f, 0.0f);
		mesh.mVertexPositions[2] = F32x3(0.0f, 0.0f, 1.0f);
		mesh.mVertexPositions[3] = F32x3(1.0f, 0.0f, 1.0f);

		// texcoords
		mesh.mVertexTexcoords.resize(vertexCount);
		mesh.mVertexTexcoords[0] = F32x2(0.0f, 1.0f);
		mesh.mVertexTexcoords[1] = F32x2(1.0f, 1.0f);
		mesh.mVertexTexcoords[2] = F32x2(0.0f, 0.0f);
		mesh.mVertexTexcoords[3] = F32x2(1.0f, 0.0f);

		// normals
		mesh.mVertexNormals.resize(vertexCount);
		for (int i = 0; i < 4; i++) {
			mesh.mVertexNormals[i] = F32x3(0.0f, 1.0f, 0.0f);
		}

		// index
		mesh.mIndices = {0,1,2,1,3,2 };

		// mesh subset
		auto& subset = mesh.mSubsets.emplace_back();
		subset.mIndexCount = 6;
		subset.mIndexOffset = 0;
		subset.mMaterialID = mMaterialEntity;

		mesh.SetupRenderData(Renderer::GetDevice());
	}

	std::shared_ptr<GameMapGround> GameMapGroundRenderer::CreateGroundObject(Scene& scene)
	{
		auto groundObject = std::make_shared<GameMapGround>();
		groundObject->LoadFromScene(scene);
		groundObject->SetMesh(mMeshEntity);
		
		auto object = groundObject->GetObjectComponent();
		if (object != nullptr) { 
			object->RenderInstanceHandler = [groundObject](RenderInstance& instance) 
			{
				U32 tileIndex = groundObject->GetTileIndex();
				instance.SetUserdata(32, 0, 16, 16);
			};
		}

		return groundObject;
	}
}