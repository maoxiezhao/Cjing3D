#include "GameMapGroundRenderer.h"
#include "resource\resourceManager.h"
#include "renderer\preset\renderPreset.h"
#include "renderer\shaderLib.h"
#include "definitions\gameRenderCommon.h"

namespace CjingGame
{
	namespace {
		GPUBuffer mGroundTilesetBuffer;
		GameMapGroundTileset* mGameMapGroundTileset = nullptr;
		std::vector<GameMapGroundInstPtr> mInstances;

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		void InitializeRender()
		{
			// initialize pipeline state
			RenderPreset& renderPreset = Renderer::GetRenderPreset();
			// static 
			PipelineStateDesc desc = {};
			desc.mInputLayout       = renderPreset.GetVertexLayout(InputLayoutType_ObjectAll);
			desc.mVertexShader      = renderPreset.GetVertexShader(VertexShaderType_ObjectAll);
			desc.mPixelShader       = Renderer::LoadShader(SHADERSTAGES_PS, "mapGroundPS.cso");
			desc.mBlendState        = renderPreset.GetBlendState(BlendStateID_Opaque);
			desc.mDepthStencilState = renderPreset.GetDepthStencilState(DepthStencilStateID_DepthReadEqual);
			desc.mRasterizerState   = renderPreset.GetRasterizerState(RasterizerStateID_Front);
			desc.mPrimitiveTopology = TRIANGLELIST;
			Renderer::RegisterCustomPipelineState(RenderPassType_TiledForward, "StaticMapGroundPSO", desc);

			// dynamic
			desc.mInputLayout  = renderPreset.GetVertexLayout(InputLayoutType_ObjectAll);
			desc.mVertexShader = Renderer::LoadShader(SHADERSTAGES_VS, "mapGroundVS.cso");
			desc.mPixelShader  = Renderer::LoadShader(SHADERSTAGES_PS, "mapGroundPS.cso");
			desc.mBlendState   = renderPreset.GetBlendState(BlendStateID_Opaque);
			desc.mDepthStencilState = renderPreset.GetDepthStencilState(DepthStencilStateID_DepthReadEqual);
			desc.mRasterizerState = renderPreset.GetRasterizerState(RasterizerStateID_Front);
			desc.mPrimitiveTopology = TRIANGLELIST;
			Renderer::RegisterCustomPipelineState(RenderPassType_TiledForward, "DynamicMapGroundPSO", desc);

			// initialize constant buffer
			const HRESULT result = CreateDynamicConstantBuffer(Renderer::GetDevice(), mGroundTilesetBuffer, sizeof(GroundTileSetCB));
			Debug::ThrowIfFailed(result, "failed to create font constant buffer:%08x", result);
			Renderer::GetDevice().SetResourceName(mGroundTilesetBuffer, "mGroundTileset");
		}

		ECS::Entity CreateMaterial(bool isStaticGround = true)
		{
			Scene& scene = Scene::GetScene();

			auto materialEntity = INVALID_ENTITY;
			// initialize static material
			if (isStaticGround)
			{
				materialEntity = scene.CreateEntityMaterial("GroundMaterial");
				MaterialComponent* material = scene.mMaterials.GetComponent(materialEntity);
				material->SetAlphaCutRef(0.2f);
				material->SetCustomShader("StaticMapGroundPSO");

				if (mGameMapGroundTileset != nullptr)
				{
					material->mBaseColorMapName = mGameMapGroundTileset->GetBaseColorTilesetPath();
					material->mBaseColorMap = mGameMapGroundTileset->GetBaseColorTexture();
				}
			}
			else
			{
				// initialize dynamic material
				materialEntity = scene.CreateEntityMaterial("DynamicGroundMaterial");
				MaterialComponent* material = scene.mMaterials.GetComponent(materialEntity);
				material->SetAlphaCutRef(0.2f);
				material->SetCustomShader("DynamicMapGroundPSO");

				if (mGameMapGroundTileset != nullptr)
				{
					material->mBaseColorMapName = mGameMapGroundTileset->GetBaseColorTilesetPath();
					material->mBaseColorMap = mGameMapGroundTileset->GetBaseColorTexture();
				}
			}

			return materialEntity;
		}

		ECS::Entity CreateMesh(ECS::Entity materialEntity, bool isStaticGround = true)
		{
			Scene& scene = Scene::GetScene();
			auto meshEntity = INVALID_ENTITY;
			// initialize static mesh component
			if (isStaticGround)
			{
				meshEntity = scene.CreateEntityMesh("GroundMesh");
				MeshComponent& mesh = *scene.mMeshes.GetComponent(meshEntity);

				// mesh subset
				auto& subset = mesh.mSubsets.emplace_back();
				subset.mIndexCount = 0;
				subset.mIndexOffset = 0;
				subset.mMaterialID = materialEntity;
			}
			else
			// initialize dynamic mesh component
			{
				meshEntity = scene.CreateEntityMesh("DynamicGroundMesh");
				MeshComponent& mesh = *scene.mMeshes.GetComponent(meshEntity);

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
				mesh.mIndices = { 0,1,2,1,3,2 };

				// mesh subset
				auto& subset = mesh.mSubsets.emplace_back();
				subset.mIndexCount = 6;
				subset.mIndexOffset = 0;
				subset.mMaterialID = materialEntity;
				mesh.SetupRenderData(Renderer::GetDevice());
			}

			return meshEntity;
		}

		ECS::Entity CreateObject(ECS::Entity meshEntity)
		{
			Scene& scene = Scene::GetScene();
			auto objectEntity = scene.CreateEntityObject("Grounds");
			ObjectComponent& object = *scene.mObjects.GetComponent(objectEntity);
			object.mMeshID = meshEntity;
			scene.SetEntityLayerMask(objectEntity, GameObjectLayerMask_Ground);

			return objectEntity;
		}
	}

	bool GameMapGroundInst::IsValid() const
	{
		return mObjectEntity != INVALID_ENTITY || mMaterialEntity != INVALID_ENTITY || mMeshEntity != INVALID_ENTITY;
	}

	void GameMapGroundInst::Clear()
	{
		Scene& scene = Scene::GetScene();
		if (mMeshEntity != INVALID_ENTITY)
		{
			MeshComponent* mesh = scene.mMeshes.GetComponent(mMeshEntity);
			if (mesh != nullptr) {
				mesh->ClearRenderData();
			}
			scene.RemoveEntity(mMeshEntity);
			mMeshEntity = INVALID_ENTITY;
		}
		if (mObjectEntity != INVALID_ENTITY) 
		{
			scene.RemoveEntity(mObjectEntity);
			mObjectEntity = INVALID_ENTITY;
		}
		if (mMaterialEntity != INVALID_ENTITY) 
		{
			scene.RemoveEntity(mMaterialEntity);
			mMaterialEntity = INVALID_ENTITY;
		}
	}

	void GameMapGroundRenderer::Initialize()
	{
		InitializeRender();
	}

	void GameMapGroundRenderer::Uninitilize()
	{
		mGroundTilesetBuffer.Clear();
		for (auto& inst : mInstances) {
			inst->Clear();
		}
		mInstances.clear();
	}

	void GameMapGroundRenderer::SetCurrentTileset(GameMapGroundTileset* tileset)
	{
		if (tileset == mGameMapGroundTileset) {
			return;
		}
		mGameMapGroundTileset = tileset;

		if (tileset != nullptr) 
		{
			for (const auto& inst : mInstances)
			{
				MaterialComponent& material = *Scene::GetScene().mMaterials.GetComponent(inst->mMaterialEntity);
				material.mBaseColorMapName = mGameMapGroundTileset->GetBaseColorTilesetPath();
				material.mBaseColorMap = mGameMapGroundTileset->GetBaseColorTexture();
			}
		}
	}

	GameMapGroundInstPtr GameMapGroundRenderer::CreateStaticInst()
	{
		auto& inst = mInstances.emplace_back(std::make_shared<GameMapGroundInst>());
		inst->mMaterialEntity = CreateMaterial(true);
		inst->mMeshEntity = CreateMesh(inst->mMaterialEntity);
		inst->mObjectEntity = CreateObject(inst->mMeshEntity);
		return inst;
	}

	void GameMapGroundRenderer::RemoveStaticInst(GameMapGroundInstPtr inst)
	{
		auto it = std::find(mInstances.begin(), mInstances.end(), inst);
		if (it != mInstances.end())
		{
			(*it)->Clear();
			mInstances.erase(it);
		}
	}

	void GameMapGroundRenderer::GenerateMesh(ECS::Entity meshEntity, std::map<I32x3, std::shared_ptr<GameMapGround>>& grounds)
	{
		if (mGameMapGroundTileset == nullptr) {
			return;
		}

		Scene& scene = Scene::GetScene();
		MeshComponent* mesh = scene.mMeshes.GetComponent(meshEntity);
		if (mesh == nullptr) {
			return;
		}

		const size_t vertexCount = 4;
		const size_t totalVertexCount = vertexCount * grounds.size();
		mesh->mVertexPositions.resize(totalVertexCount);
		mesh->mVertexTexcoords.resize(totalVertexCount);
		mesh->mVertexNormals.resize(totalVertexCount);

		const size_t totalIndicesCount = grounds.size() * 6;
		mesh->mIndices.resize(totalIndicesCount);

		GameMapGroundTileset* tileset = mGameMapGroundTileset;
		auto GenerateGroundMesh = [mesh, tileset](size_t index, GameMapGround& ground, const I32x3& pos)
		{
			// position
			F32 xOffset = (F32)pos.x();
			F32 yOffset = (F32)pos.y();
			F32 zOffset = (F32)pos.z();

			size_t vertexOffset = index * 4;
			mesh->mVertexPositions[vertexOffset + 0] = F32x3(xOffset, yOffset, zOffset);
			mesh->mVertexPositions[vertexOffset + 1] = F32x3(xOffset + 1.0f, yOffset, zOffset);
			mesh->mVertexPositions[vertexOffset + 2] = F32x3(xOffset, yOffset, zOffset + 1.0f);
			mesh->mVertexPositions[vertexOffset + 3] = F32x3(xOffset + 1.0f, yOffset, zOffset + 1.0f);

			// texcoords	
			F32x4 texCoores = tileset->GetTileRectByIndex(ground.GetTileIndex());
			mesh->mVertexTexcoords[vertexOffset + 0] = { texCoores[0], texCoores[3] };
			mesh->mVertexTexcoords[vertexOffset + 1] = { texCoores[2], texCoores[3] };
			mesh->mVertexTexcoords[vertexOffset + 2] = { texCoores[0], texCoores[1] };
			mesh->mVertexTexcoords[vertexOffset + 3] = { texCoores[2], texCoores[1] };

			// normals
			for (int i = 0; i < 4; i++) {
				mesh->mVertexNormals[vertexOffset + i] = F32x3(0.0f, 1.0f, 0.0f);
			}

			// index
			mesh->mIndices[index * 6 + 0] = vertexOffset + 0;
			mesh->mIndices[index * 6 + 1] = vertexOffset + 1;
			mesh->mIndices[index * 6 + 2] = vertexOffset + 2;
			mesh->mIndices[index * 6 + 3] = vertexOffset + 1;
			mesh->mIndices[index * 6 + 4] = vertexOffset + 3;
			mesh->mIndices[index * 6 + 5] = vertexOffset + 2;
		};

		size_t index = 0;
		for (const auto& kvp : grounds) {
			GenerateGroundMesh(index++, *kvp.second, kvp.first);
		}

		auto& subset = mesh->mSubsets.back();
		subset.mIndexCount = totalIndicesCount;
		mesh->SetupRenderData(Renderer::GetDevice());
	}

	void GameMapGroundRenderer::PreRender()
	{
		if (mGameMapGroundTileset != nullptr)
		{
			auto tilesetSize = mGameMapGroundTileset->GetTilesetSize();
			GroundTileSetCB cb;
			cb.gGroundTilesetWidth = tilesetSize[0];
			cb.gGroundTilesetHeight = tilesetSize[1];
			cb.gGroundTilesetInvWidth = 1.0f / (F32)cb.gGroundTilesetWidth;
			cb.gGroundTilesetInvHeight = 1.0f / (F32)cb.gGroundTilesetHeight;

		/*	Renderer::GetDevice().UpdateBuffer(mGroundTilesetBuffer, &cb);
			Renderer::GetDevice().BindConstantBuffer(SHADERSTAGES_VS, mGroundTilesetBuffer, CB_GETSLOT_NAME(GroundTileSetCB));*/
		}
	}
}