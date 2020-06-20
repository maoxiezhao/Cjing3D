#include "terrainTree.h"
#include "terrainPass.h"
#include "system\sceneSystem.h"
#include "resource\resourceManager.h"
#include "renderer\rhiResourceManager.h"
#include "renderer\RHI\rhiFactory.h"

#include <stack>

namespace Cjing3D {
namespace {

	// 更具当前距离获取Lod等级，目前直接采用了线性关系，后续可以优化
	U32 GetLodLevelByDistance(F32 distance, U32 maxLevel, U32 tileVertexCount)
	{
		int dist = (int)distance;
		for (int level = (int)maxLevel; level >= 0; level--) 
		{
			if ((dist >>= 1) < (tileVertexCount)) return level;
		}
		return 0;
	}

	std::vector<TerrainTile*> mCulledTerrainTiles;
}	

void TerrainMaterial::Clear()
{
	weightTexture = nullptr;
	detailTexture1 = nullptr;
	detailTexture2 = nullptr;
	detailTexture3 = nullptr;
}

TerrainTile::TerrainTile() 
{
}

TerrainTile::~TerrainTile()
{
}

U32 TerrainTile::GetIdentityHashValue() const
{
	return U32();
}

U32 TerrainTree::TerrainTileQuadTreeNode::GetEdgeLodLevel(NodeDirection direction)
{
	U32 index = static_cast<U32>(direction);
	return mEdgeLodLevel[index];
}

void TerrainTree::TerrainTileQuadTreeNode::SetEdgeLodLevel(NodeDirection direction, U32 level)
{
	U32 index = static_cast<U32>(direction);
	mEdgeLodLevel[index] = level;
}

U32 TerrainTree::TerrainTileQuadTreeNode::GetAllEdgeLodLevel() const
{
	U32 value = 0;
	for (U32 index = 0; index < NodeDirection::NodeDirection_Count; index++) {
		value |= (mEdgeLodLevel[index]) << (8 * index);
	}
	return value;
}

TerrainTree::TerrainTree() 
{
}

TerrainTree::~TerrainTree()
{
}

void TerrainTree::Initialize(U32 width, U32 height)
{
	mTerrainWidth = width;
	mTerrainHeight = height;

	// initialize constant buffer
	GraphicsDevice& device = Renderer::GetDevice();
	GPUBufferDesc desc = {};
	desc.mUsage = USAGE_DEFAULT;
	desc.mCPUAccessFlags = 0;
	desc.mBindFlags = BIND_CONSTANT_BUFFER;
	desc.mByteWidth = sizeof(TerrainCB);

	const auto result = device.CreateBuffer(&desc, mTerrainBuffer, nullptr);
	Debug::ThrowIfFailed(result, "failed to create terrain constant buffer:%08x", result);
	device.SetResourceName(mTerrainBuffer, "TerrainCB");

	mTerrainQuadTree.Setup({ 0.0f, 0.0f, (F32)mTerrainWidth, (F32)mTerrainWidth });

	// 初始化tileManager,设置一个tile的默认顶点数为32
	mTerrainTileManager.Initialize(16);

	// 计算maxLodLevel
	U32 maxSize = std::max(mTerrainWidth, mTerrainHeight);
	U32 tileVertexCount = mTerrainTileManager.GetTerrainTileVertexCount();
	mTerrainMaxLodLevel = (U32)std::log2f((F32)maxSize) - (U32)std::log2f((F32)tileVertexCount) - 1;

	// 设置QuadTree的最大深度
	mTerrainQuadTree.SetMaxDepth(mTerrainMaxLodLevel);
}

void TerrainTree::Uninitialize()
{
	mTerrainTileManager.Uninitialize();
	mTerrainQuadTree.Clear();
}

void TerrainTree::UpdatePerFrameData(F32 deltaTime)
{
	// update terrain tree
	UpdateTerrainTree(Renderer::GetCamera());
}

void TerrainTree::RefreshRenderData(TransformComponent& transform)
{
	if (mTerrainBufferDirty) {
		UpdateConstantBuffer(transform);
	}
}

void TerrainTree::Render()
{
	GraphicsDevice& device = Renderer::GetDevice();
	device.BeginEvent("RenderTerrainTree");

	TerrrainRenderQueue renderQueue;
	for (auto& terrainTile : mCulledTerrainTiles)
	{
		renderQueue.AddBatch(terrainTile);
	}

	if (!renderQueue.IsEmpty())
	{
		ProcessTerrainRenderQueue(renderQueue);
	}

	device.EndEvent();
}

void TerrainTree::LoadHeightMap(const std::string& path)
{
	ResourceManager& resourceManager = GlobalGetSubSystem<ResourceManager>();
	mHeightMap = resourceManager.GetOrCreate<TextureResource>(path, FORMAT_R8_UNORM, 1);
}

void TerrainTree::LoadHeightMap(TextureResourcePtr heightMap)
{
	mHeightMap = heightMap;
}

TextureResourcePtr TerrainTree::GetHeightMap()
{
	return mHeightMap;
}

void TerrainTree::LoadTerrainMaterial(TerrainMaterial material)
{
	mTerrainMaterial = material;
}

void TerrainTree::SetElevation(U32 elevation)
{
	mTerrainElevation = elevation;
	mTerrainBufferDirty = true;
}

void TerrainTree::ProcessTerrainRenderQueue(TerrrainRenderQueue& renderQueue)
{
	GraphicsDevice& device = Renderer::GetDevice();

	U32 batchCount = renderQueue.GetCount();
	LinearAllocator& frameAllocator = Renderer::GetFrameAllocator(Renderer::FrameAllocatorType_Render);
	size_t instanceSize = sizeof(RenderTerrainInstance);
	GraphicsDevice::GPUAllocation instances = device.AllocateGPU(batchCount * instanceSize);

	// build render batches
	U32 prevIdentityHash = -1;
	U32 instancedBatchCount = 0;

	std::vector<TerrainRenderBatchInstance*> mRenderBatchInstances;
	auto& renderTerrainTiles = renderQueue.mRenderTiles;
	for (U32 index = 0; index < batchCount; index++)
	{
		TerrainTile& terrainTile = *renderTerrainTiles[index];
		U32 identityHash = terrainTile.GetIdentityHashValue();

		if (identityHash != prevIdentityHash)
		{
			prevIdentityHash = identityHash;
			instancedBatchCount++;

			TerrainRenderBatchInstance* batchInstance = (TerrainRenderBatchInstance*)frameAllocator.Allocate(sizeof(TerrainRenderBatchInstance));
			batchInstance->mVertexBuffer = terrainTile.GetVertexBuffer();
			batchInstance->mIndexBuffer = terrainTile.GetIndexBuffer();
			batchInstance->mIndiceCount = terrainTile.GetIndiceCount();
			batchInstance->mDataOffset = instances.offset + index * instanceSize;
			batchInstance->mInstanceCount = 0;

			mRenderBatchInstances.push_back(batchInstance);
		}

		// process terrain tile rect
		Rect terrainRect = terrainTile.GetRect();
		U32 cellVertexCount = mTerrainTileManager.GetTerrainTileVertexCount();
		F32 cellScale = terrainRect.GetSize()[0] / cellVertexCount;	// 必须保证宽和高一致

		// 处理地形lod
		U32 currentLodLevel = terrainTile.GetLodLevel();
		U32 edgeLodLevel = terrainTile.GetEdgeLodLevel();

		// 记录当前tile的局部位移信息，因为transform是作用于整个tree，
		// 所以这些局部信息需要单独储存
		const XMFLOAT4 localTransform(
			terrainRect.mLeft,
			terrainRect.mTop,
			cellScale, 
			1.0f);

		// 保存每个batch（包含一个object）的worldMatrix
		// 用于在shader中计算顶点的世界坐标
		((RenderTerrainInstance*)instances.data)[index].Setup(localTransform, currentLodLevel, edgeLodLevel, cellVertexCount);

		mRenderBatchInstances.back()->mInstanceCount++;
	}

	// TODO： 现在的绘制过程太过繁琐，后续会优化封装
	Renderer::BindConstanceBuffer(SHADERSTAGES_DS);

	device.BindConstantBuffer(SHADERSTAGES_VS, mTerrainBuffer, CBSLOT_TERRAIN);
	device.BindConstantBuffer(SHADERSTAGES_DS, mTerrainBuffer, CBSLOT_TERRAIN);
	device.BindConstantBuffer(SHADERSTAGES_PS, mTerrainBuffer, CBSLOT_TERRAIN);

	for (auto& batchInstance : mRenderBatchInstances)
	{
		GPUBuffer* vertexBufferPos = batchInstance->mVertexBuffer;
		GPUBuffer* indexBuffer = batchInstance->mIndexBuffer;
		U32 indiceCount = batchInstance->mIndiceCount;

		GPUBuffer* vbs[] = {
			vertexBufferPos,
			instances.buffer
		};
		U32 strides[] = {
			sizeof(MeshComponent::VertexPosNormalSubset),
			(U32)instanceSize
		};
		U32 offsets[] = {
			0,
			batchInstance->mDataOffset
		};
		device.BindVertexBuffer(vbs, 0, ARRAYSIZE(vbs), strides, offsets);
		device.BindIndexBuffer(*indexBuffer, IndexFormat::INDEX_FORMAT_32BIT, 0);
		device.BindPipelineState(mTerrainPSO);

		// bind domain shader resource
		GPUResource* vsResources[] = {
			mHeightMap != nullptr ? mHeightMap->mTexture : nullptr,
		};
		device.BindGPUResources(SHADERSTAGES_DS, vsResources, TEXTURE_SLOT_0, 1);

		// bind pixel shader resources
		GPUResource* psResources[] = {
			mHeightMap != nullptr ? mHeightMap->mTexture : nullptr,
			mTerrainMaterial.weightTexture != nullptr  ? mTerrainMaterial.weightTexture->mTexture : nullptr,
			mTerrainMaterial.detailTexture1 != nullptr ? mTerrainMaterial.detailTexture1->mTexture : nullptr,
			mTerrainMaterial.detailTexture2 != nullptr ? mTerrainMaterial.detailTexture2->mTexture : nullptr,
			mTerrainMaterial.detailTexture3 != nullptr ? mTerrainMaterial.detailTexture3->mTexture : nullptr,
		};
		device.BindGPUResources(SHADERSTAGES_PS, psResources, TEXTURE_SLOT_0, ARRAYSIZE(psResources));

		// draw instances
		device.DrawIndexedInstances(indiceCount, batchInstance->mInstanceCount, 0, 0, 0);
	}

	device.UnAllocateGPU();
	frameAllocator.Free(sizeof(RenderBatchInstance) * instancedBatchCount);
}

void TerrainTree::UpdateConstantBuffer(TransformComponent& transform)
{
	TerrainCB cb;
	cb.gTerrainTransform = transform.GetWorldTransform();
	cb.gTerrainResolution.x = mTerrainWidth;
	cb.gTerrainResolution.y = mTerrainHeight;
	cb.gTerrainInverseResolution.x = (1.0f / mTerrainWidth);
	cb.gTerrainInverseResolution.y = (1.0f / mTerrainHeight);
	cb.gTerrainElevation = (F32)mTerrainElevation;
	cb.gTerrainHaveWeightDetailMap = (mTerrainMaterial.weightTexture != nullptr) ? 1 : 0;

	Renderer::GetDevice().UpdateBuffer(mTerrainBuffer, &cb, sizeof(TerrainCB));
}

void TerrainTree::UpdateTerrainTree(CameraComponent& camera)
{
	mTerrainQuadTree.Clear();
	U32 treeNodeCount = 0;

	LinearAllocator& frameAllocator = Renderer::GetFrameAllocator(Renderer::FrameAllocatorType_Render);

	std::stack<TerrainTreeNode*> currentTreeNodes;
	auto& rootNode = mTerrainQuadTree.GetRootNode();
	mTerrainQuadTree.SplitNode(rootNode, frameAllocator);

	for (auto node : rootNode.GetChildren()) {
		currentTreeNodes.push(node);
	}

	while (!currentTreeNodes.empty())
	{
		TerrainTreeNode& currentNode = *currentTreeNodes.top();
		currentTreeNodes.pop();

		if (!CheckTerrainNodeCanSplit(camera, currentNode)) {
			continue;
		}

		mTerrainQuadTree.SplitNode(currentNode, frameAllocator);

		for (auto node : currentNode.GetChildren()) {
			currentTreeNodes.push(node);
		}
	}

	// seamless terrain
	CalcSeamlessLevel();
	// 需要做第二次，来保证每个节点都更新过edge lod level
	CalcSeamlessLevel();	

	// get terrain tiles
	mCulledTerrainTiles.clear();
	auto allNodes = mTerrainQuadTree.GetAllNodes();
	for (auto& node : allNodes)
	{
		if (!node->IsLeafNode()) {
			continue;
		}

		Rect rect = node->GetRect();
		U32 level = node->GetDepth();
		U32x2 localPos = node->GetLocalPos();
		U32 edgeLodLevel = node->GetData().GetAllEdgeLodLevel();
		
		TerrainTilePtr tilePtr = mTerrainTileManager.GetTerrainTile(localPos, level, rect, edgeLodLevel);
		node->GetData().mTilePtr = tilePtr;

		mCulledTerrainTiles.push_back(&(*tilePtr));
	}

	mTerrainQuadTree.Clear();
	frameAllocator.Free(sizeof(TerrainTreeNode) * treeNodeCount);
}

void TerrainTree::CalcSeamlessLevel()
{
	std::stack<TerrainTreeNode*> currentTreeNodes;
	auto& rootNode = mTerrainQuadTree.GetRootNode();
	for (auto node : rootNode.GetChildren()) {
		currentTreeNodes.push(node);
	}

	auto ProcessEdgeLodLevel = [&](NodeDirection direction, TerrainTreeNode& currentNode)
	{
		TerrainTreeNode* targetNode = nullptr;
		switch (direction)
		{
		case NodeDirection_NORTH:
			targetNode = mTerrainQuadTree.GetNorthNeighbor(currentNode);
			break;
		case NodeDirection_EAST:
			targetNode = mTerrainQuadTree.GetEastNeighbor(currentNode);
			break;
		case NodeDirection_SOUTH:
			targetNode = mTerrainQuadTree.GetSouthNeighbor(currentNode);
			break;
		case NodeDirection_WEST:
			targetNode = mTerrainQuadTree.GetWestNeighbor(currentNode);
			break;
		}

		if (targetNode == nullptr)
		{
			currentNode.GetData().SetEdgeLodLevel(direction, currentNode.GetDepth());
		}
		else
		{
			NodeDirection oppositeDir = GetOppositeNodeDirection(direction);
			auto& currentData = currentNode.GetData();
			auto& targetData = targetNode->GetData();

			if (currentData.GetEdgeLodLevel(direction) < currentNode.GetDepth()) {
				currentData.SetEdgeLodLevel(direction, currentNode.GetDepth());
			}

			if (targetData.GetEdgeLodLevel(oppositeDir) < currentData.GetEdgeLodLevel(direction)) {
				targetData.SetEdgeLodLevel(oppositeDir, currentData.GetEdgeLodLevel(direction));
			}

			if (currentData.GetEdgeLodLevel(direction) < targetData.GetEdgeLodLevel(oppositeDir)) {
				currentData.SetEdgeLodLevel(direction, targetData.GetEdgeLodLevel(oppositeDir));
			}
		}
	};

	while (!currentTreeNodes.empty())
	{
		TerrainTreeNode& currentNode = *currentTreeNodes.top();
		currentTreeNodes.pop();

		if (!currentNode.IsLeafNode())
		{
			for (auto node : currentNode.GetChildren()) {
				currentTreeNodes.push(node);
			}
		}
		else
		{
			for (U32 dir = 0; dir < NodeDirection::NodeDirection_Count; dir++) {
				ProcessEdgeLodLevel(static_cast<NodeDirection>(dir), currentNode);
			}
		}
	}
}

bool TerrainTree::CheckTerrainNodeCanSplit(CameraComponent& camera, TerrainTreeNode& treeNode)
{
	U32 currentLevel = treeNode.GetDepth();
	if (currentLevel >= mTerrainMaxLodLevel) {
		return false;
	}

	// build node aabb and frustum culling
	Rect nodeRect = treeNode.GetRect();
	F32x3 minPos = { nodeRect.mLeft, 0.0f, nodeRect.mTop};
	F32x3 maxPos = { nodeRect.mRight, (F32)mTerrainElevation, nodeRect.mBottom };
	AABB aabb(minPos, maxPos);
	if (!camera.GetFrustum().Overlaps(aabb))
	{
		return false;
	}

	// 目前quad的拆分评估，仅考虑平面位置关系，暂时不考虑地形复杂度
	// 以后需要先转换到世界坐标系中
	const F32x2 rectCenter = treeNode.GetRect().GetCenter();
	const F32x3 center = F32x3(rectCenter[0], 0.0f, rectCenter[1]);
	F32x3 cameraPos = camera.GetCameraPos();
	cameraPos[1] = 0.0f;

	F32 distance = DistanceEstimated(cameraPos, center);
	U32 lodLevel = GetLodLevelByDistance(distance, mTerrainMaxLodLevel, mTerrainTileManager.GetTerrainTileVertexCount());
	
	if (lodLevel <= currentLevel) {
		return false;
	}

	return true;
}

TerrainTileManager::TerrainTileManager()
{
}

TerrainTileManager::~TerrainTileManager()
{
	Clear();
}

void TerrainTileManager::Initialize(U32 tileVertexCount)
{
	mTerrainTileVertexCount = tileVertexCount;

	U32 patchWidth = tileVertexCount + 1;
	U32 patchHeight = tileVertexCount + 1;

	U32 arraySize = patchWidth * patchHeight;
	mVertexPositions.resize(arraySize);

	for (U32 y = 0; y < patchWidth; y++)
	{
		for (U32 x = 0; x < patchHeight; x++)
		{
			U32 index = y * patchWidth + x;
			mVertexPositions[index] = F32x3(
				(F32)(x),
				0.0f,
				(F32)(y)
			);
		}
	}

	// use triangle list
	U32 indiceSize = (patchHeight - 1) * (patchWidth - 1) * 4;
	mIndices.resize(indiceSize);

	U32 indiceIndex = 0;
	for (U32 y = 0; y < patchHeight - 1; y++)
	{
		U32 vertexIndex = y * patchWidth;
		for (U32 x = 0; x < patchWidth - 1; x++)
		{
			U32 currentVertex = vertexIndex + x;

			mIndices[indiceIndex++] = currentVertex;
			mIndices[indiceIndex++] = currentVertex + 1;
			mIndices[indiceIndex++] = currentVertex + patchWidth;
			mIndices[indiceIndex++] = currentVertex + patchWidth + 1;
		}
	}

	UpdateGeometryBuffer();
}

void TerrainTileManager::Uninitialize()
{
	Clear();
	mVertexPositions.clear();
	mIndices.clear();
}

TerrainTilePtr TerrainTileManager::GetTerrainTile(const U32x2& locaPos, U32 depth, const Rect& rect, U32 edgeLodLevel)
{
	const U32 key = (depth * 1000000000) + (locaPos[1] * 10000) + locaPos[0];

	TerrainTilePtr ret = nullptr;
	auto it = mTerrainTileMap.find(key);
	if (it != mTerrainTileMap.end()) 
	{
		ret = it->second;
	}
	else
	{
		ret = std::make_shared<TerrainTile>();
		ret->SetIndiceCount(mIndices.size());
		ret->SetVertexBuffer(&mVertexBufferPos);
		ret->SetIndexBuffer(&mIndexBuffer);

		mTerrainTileMap[key] = ret;
	}

	ret->SetRect(rect);
	ret->SetLocalPos(locaPos);
	ret->SetLodLevel(depth);
	ret->SetEdgeLodLevel(edgeLodLevel);

	return ret;
}

void TerrainTileManager::Clear()
{
	mTerrainTileMap.clear();
}

void TerrainTileManager::UpdateGeometryBuffer()
{
	GraphicsDevice& device = Renderer::GetDevice();
	{
		const auto result = CreateStaticIndexBuffer(device, mIndexBuffer, mIndices);
		Debug::ThrowIfFailed(result, "Failed to create index buffer:%08x", result);
	}

	F32x3 mMinPos = { FLT_MAX,  FLT_MAX,  FLT_MAX };
	F32x3 mMaxPos = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
	{
		std::vector<MeshComponent::VertexPosNormalSubset> vertices(mVertexPositions.size());
		for (int i = 0; i < mVertexPositions.size(); i++)
		{
			F32x3 pos = mVertexPositions[i];
			F32x3 normal = F32x3(0.0f, 0.0f, 0.0f);
			normal = F32x3Normalize(normal);

			auto& vertex = vertices[i];
			vertex.Setup(pos, normal, 0);

			mMinPos = F32x3Min(mMinPos, pos);
			mMaxPos = F32x3Max(mMaxPos, pos);
		}

		const auto result = CreateBABVertexBuffer(device, mVertexBufferPos, vertices);
		Debug::ThrowIfFailed(result, "Failed to create vertex buffer:%08x", result);
	}
}
}