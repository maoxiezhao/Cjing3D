#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\renderer.h"
#include "system\component\camera.h"
#include "utils\quadTree.h"
#include "utils\geometry.h"

namespace Cjing3D
{
	class Renderer;
	class TerrainPass;

	// 地形渲染批次
	struct TerrainRenderBatchInstance
	{
		GPUBuffer* mVertexBuffer = nullptr;
		GPUBuffer* mIndexBuffer = nullptr;

		U32 mIndiceCount = 0;
		U32 mDataOffset = 0;
		U32 mInstanceCount = 0;
	};

	// 地形材质结构
	struct TerrainMaterial
	{
		Texture2DPtr weightTexture;
		Texture2DPtr detailTexture1;
		Texture2DPtr detailTexture2;
		Texture2DPtr detailTexture3;

		void Clear();
	};

	//--------------------------------------------------------------------
	// 地形瓷片，一个地形渲染的基本单元
	class TerrainTile
	{
	public:
		TerrainTile();
		~TerrainTile();

		U32 GetIndiceCount() const { return mIndiceCount; }
		U32 GetIdentityHashValue() const;
		Rect GetRect()const { return mTerrainRect; }
		U32x2 GetLocalPos()const { return mLocalPos; }
		U32 GetLodLevel()const { return mLodLevel; }

		void SetLodLevel(U32 level) { mLodLevel = level; }
		void SetRect(const Rect& rect) { mTerrainRect = rect; }
		void SetLocalPos(const U32x2& pos) { mLocalPos = pos; }
		void SetVertexBuffer(GPUBuffer* buffer) { mVertexBufferPos = buffer; }
		void SetIndexBuffer(GPUBuffer* buffer) { mIndexBuffer = buffer; }
		void SetIndiceCount(U32 indiceCount) { mIndiceCount = indiceCount; }

		GPUBuffer* GetVertexBuffer() { return mVertexBufferPos; }
		GPUBuffer* GetIndexBuffer() { return mIndexBuffer; }

	private:
		Rect mTerrainRect;
		U32x2 mLocalPos = {0u, 0u};
		U32 mLodLevel = 1;
		U32 mIndiceCount = 0;

		GPUBuffer* mVertexBufferPos = nullptr;
		GPUBuffer* mIndexBuffer = nullptr;
	};
	using TerrainTilePtr = std::shared_ptr<TerrainTile>;

	//--------------------------------------------------------------------
	class TerrainTileManager
	{
	public:
		TerrainTileManager();
		~TerrainTileManager();

		void Initialize(U32 tileVertexCount = 16);
		void Uninitialize();
		void Clear();
		void UpdateGeometryBuffer();

		U32 GetTerrainTileVertexCount()const { return mTerrainTileVertexCount; }
		TerrainTilePtr GetTerrainTile(Renderer& renderer, const U32x2& locaPos, U32 depth, const Rect& rect);

	private:
		std::map<U32, TerrainTilePtr> mTerrainTileMap;

		U32 mTerrainTileVertexCount = 16;
		GPUBuffer mVertexBufferPos;
		GPUBuffer mIndexBuffer;
		std::vector<F32x3> mVertexPositions;
		std::vector<U32> mIndices;
	};

	//--------------------------------------------------------------------
	class TerrainTreeComponent
	{
	public:
		TerrainTreeComponent();
		~TerrainTreeComponent();

		virtual void Initialize(U32 width, U32 height);
		virtual void Uninitialize();
		virtual void UpdatePerFrameData(F32 deltaTime);
		virtual void RefreshRenderData();
		virtual void Render();

		void LoadHeightMap(const std::string& path);
		void LoadTerrainMaterial(TerrainMaterial material);

		U32 GetElevation()const { return mTerrainElevation; }
		void SetElevation(U32 elevation);

		PipelineState GetTerrainPSO()const { return mTerrainPSO; }
		void SetTerrainPSO(PipelineState& pso) { mTerrainPSO = pso; }

	private:
		void UpdateConstantBuffer();

		struct TerrrainRenderQueue
		{
		public:
			void AddBatch(TerrainTile* renderTile)
			{
				mRenderTiles.push_back(renderTile);
			}

			void Clear()
			{
				mRenderTiles.clear();
			}

			bool IsEmpty() const
			{
				return mRenderTiles.empty();
			}

			U32 GetCount()const
			{
				return mRenderTiles.size();
			}

			std::vector<TerrainTile*> mRenderTiles;
		};
		void ProcessTerrainRenderQueue(TerrrainRenderQueue& renderQueue);

	private:
		PipelineState mTerrainPSO;

		U32 mTerrainWidth = 256;
		U32 mTerrainHeight = 256;
		U32 mTerrainElevation = 10;
		U32 mTerrainMaxLodLevel = 8;

		TerrainMaterial mTerrainMaterial;
		GPUBuffer mTerrainBuffer;
		bool mTerrainBufferDirty = true;
		Texture2DPtr mHeightMap = nullptr;

		TerrainTileManager mTerrainTileManager;

		// terrain quad tree
		using TerrainTreeNode = QuadTreeNode<TerrainTilePtr>;
		using TerrainQuadTree = QuadTree<TerrainTilePtr>;

		void UpdateTerrainTree(CameraComponent& camera);
		void CalcSeamlessLevel();
		bool CheckTerrainNodeCanSplit(CameraComponent& camera, TerrainTreeNode& treeNode);

		TerrainQuadTree mTerrainQuadTree;
	};
}