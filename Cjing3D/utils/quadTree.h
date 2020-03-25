#pragma once

#include "utils\math.h"
#include "utils\allocator.h"

#include <map>
#include <set>
#include <functional>

namespace Cjing3D
{
	// 当前实现的四叉树，暂时不支持自动分区合并，只是用于地形的
	// LOD计算，保持四叉树的节点结构
	// TODO: 重构代码，支持完整四叉树

	enum NodeDirection 
	{ 
		NodeDirection_WEST,
		NodeDirection_NORTH,
		NodeDirection_EAST,
		NodeDirection_SOUTH,
		NodeDirection_Count
	};
	inline NodeDirection GetOppositeNodeDirection(NodeDirection direction)
	{
		switch (direction)
		{
		case Cjing3D::NodeDirection_NORTH:
			return NodeDirection_SOUTH;
		case Cjing3D::NodeDirection_EAST:
			return NodeDirection_WEST;
		case Cjing3D::NodeDirection_SOUTH:
			return NodeDirection_NORTH;
		case Cjing3D::NodeDirection_WEST:
			return NodeDirection_EAST;
		}
	}

	template<typename T>
	class QuadTreeNode
	{
		using NodeType = QuadTreeNode<T>;
	public:
		QuadTreeNode() { Clear(); }
		QuadTreeNode(const Rect& rect) : mRect(rect) { Clear(); }

		void SetDepth(I32 depth) { mDepth = depth; }
		I32 GetDepth()const { return mDepth; }
		void SetRect(const Rect& rect) { mRect = rect; }
		Rect GetRect()const { return mRect; }
		void SetLocalPos(const U32x2& pos) { mLocalPos = pos; }
		U32x2 GetLocalPos()const { return mLocalPos; }
		void SetData(T value) { mElement = value; }
		T& GetData() { return mElement; }

		bool IsLeafNode()const { return !IsSplit(); }
		bool IsSplit()const;
		void Split(LinearAllocator& allocator);
		void Clear();

		std::array<NodeType*, 4>& GetChildren() { return mChildren; }

	private:
		U32x2 mLocalPos = {0u, 0u}; // 节点在当前层级的局部坐标
		I32 mDepth = 0;
		Rect mRect;
		T mElement;
		std::array<NodeType*, 4> mChildren;
	};

	template<typename T>
	struct EvaluateFunc
	{
		using Func = std::function<bool(QuadTreeNode<T>&)>;
	};
	
	template<typename T>
	class QuadTree
	{
	public:
		using NodeType = QuadTreeNode<T>;
		static const U32 MaxTreeNodeCount = 4;

		QuadTree() {}
		~QuadTree() { Clear();}

		void Setup(const Rect& rect);
		void Clear();
		void InsertNode(QuadTreeNode<T>* node);
		bool Insert(const T& element, const Rect& boundingBox);
		bool Remove(const T& element, const Rect& boundingBox);
		void SplitNode(QuadTreeNode<T>& node, LinearAllocator& allocator);
		void SetMaxDepth(U32 maxDepth) { mTreeMaxDepth = maxDepth; }

		QuadTreeNode<T>& GetRootNode() { return mRoot; }
		std::set<QuadTreeNode<T>*>& GetAllNodes() { return mAllNodes; }
		std::vector<T> GetAllElements();

		QuadTreeNode<T>* GetNodeByRect(const Rect& rect);
		QuadTreeNode<T>* GetNodeByPos(const F32x2& pos, U32 currentLevel);
		QuadTreeNode<T>* GetNode(U32 level, U32 localX, U32 localY);

		U32x2 ConvertToTargetLevelLoclPos(U32x2 localPos, U32 currentLevel, U32 targetLevel)
		{
			U32 shiftlevel = std::max(0u, targetLevel - currentLevel);
			localPos[0] <<= shiftlevel;
			localPos[1] <<= shiftlevel;

			return localPos;
		}

		NodeType* GetNorthNeighbor(NodeType& node)
		{
			Rect rect = node.GetRect();
			F32x2 center = rect.GetCenter();
			center[1] += (rect.GetHeight() * 0.5f + 0.5f);

			NodeType* ret = GetNodeByPos(center, node.GetDepth());
			return (ret == &node) ? nullptr : ret;
		}

		NodeType* GetSouthNeighbor(NodeType& node)
		{
			Rect rect = node.GetRect();
			F32x2 center = rect.GetCenter();
			center[1] -= (rect.GetHeight() * 0.5f + 0.5f);

			NodeType* ret = GetNodeByPos(center, node.GetDepth());
			return (ret == &node) ? nullptr : ret;
		}

		NodeType* GetEastNeighbor(NodeType& node)
		{
			Rect rect = node.GetRect();
			F32x2 center = rect.GetCenter();
			center[0] += (rect.GetWidth() * 0.5f + 0.5f);

			NodeType* ret = GetNodeByPos(center, node.GetDepth());
			return (ret == &node) ? nullptr : ret;
		}

		NodeType* GetWestNeighbor(NodeType& node)
		{
			Rect rect = node.GetRect();
			F32x2 center = rect.GetCenter();
			center[0] -= (rect.GetWidth() * 0.5f + 0.5f);

			NodeType* ret = GetNodeByPos(center, node.GetDepth());
			return (ret == &node) ? nullptr : ret;
		}

		U32 MakeHashKey(QuadTreeNode<T>& node);
		U32 MakeHashKey(U32 level, U32 localX, U32 localY);
	private:
		U32 mTreeMaxDepth = 12;
		QuadTreeNode<T> mRoot;
		std::set<QuadTreeNode<T>*> mAllNodes;

		// 使用hashmap来代替递归获取节点
		std::map<U32, QuadTreeNode<T>*> mNodeLocalHashMap;
	};

	template<typename T>
	inline void QuadTree<T>::Setup(const Rect& rect)
	{
		mRoot.SetRect(rect);
		mRoot.SetDepth(-1);
	}

	template<typename T>
	inline void QuadTree<T>::Clear()
	{
		mRoot.Clear();

		for (auto& node : mAllNodes) {
			node->Clear();
		}
		mAllNodes.clear();
		mNodeLocalHashMap.clear();
	}

	template<typename T>
	inline void QuadTree<T>::InsertNode(QuadTreeNode<T>* node)
	{
		const I32 depth = node->GetDepth();
		const U32 key = MakeHashKey(*node);
		auto it = mNodeLocalHashMap.find(key);
		if (it != mNodeLocalHashMap.end()) {
			return;
		}

		mNodeLocalHashMap[key] = node;
		mAllNodes.insert(node);
	}

	template<typename T>
	inline bool QuadTree<T>::Insert(const T& element, const Rect& boundingBox)
	{
		return false;
	}

	template<typename T>
	inline bool QuadTree<T>::Remove(const T& element, const Rect& boundingBox)
	{
		return false;
	}

	template<typename T>
	inline void QuadTree<T>::SplitNode(QuadTreeNode<T>& node, LinearAllocator& allocator)
	{
		node.Split(allocator);

		auto& chilren = node.GetChildren();
		for (auto child : chilren) {
			InsertNode(child);
		}
	}

	template<typename T>
	inline std::vector<T> QuadTree<T>::GetAllElements()
	{
		std::vector<T> ret;

		// todo

		return ret;
	}

	template<typename T>
	inline QuadTreeNode<T>* QuadTree<T>::GetNodeByRect(const Rect& rect)
	{
		return nullptr;
	}

	template<typename T>
	inline QuadTreeNode<T>* QuadTree<T>::GetNodeByPos(const F32x2& pos, U32 currentLevel)
	{
		U32 gridSize = 1 << (mTreeMaxDepth -1 );
		U32 x = std::max(0u, (U32)(pos[0] / (F32)gridSize));
		U32 y = std::max(0u, (U32)(pos[1] / (F32)gridSize));

		U32 nodeLevel = mTreeMaxDepth;
		QuadTreeNode<T>* ret = nullptr;
		do
		{
			ret = GetNode(nodeLevel, x, y);
			if (nodeLevel <= 0)
				break;

			// goto parent level
			x >>= 1;
			y >>= 1;
			--nodeLevel;
		} while (ret == nullptr);

		return ret;
	}

	template<typename T>
	inline QuadTreeNode<T>* QuadTree<T>::GetNode(U32 level, U32 localX, U32 localY)
	{
		const U32 key = MakeHashKey(level, localX, localY);
		auto it = mNodeLocalHashMap.find(key);
		if (it == mNodeLocalHashMap.end()) {
			return nullptr;
		}

		return it->second;
	}

	template<typename T>
	inline U32 QuadTree<T>::MakeHashKey(QuadTreeNode<T>& node)
	{
		U32x2 localPos = node.GetLocalPos();
		I32 level = node.GetDepth();
		return MakeHashKey(level, localPos[0], localPos[1]);
	}

	template<typename T>
	inline U32 QuadTree<T>::MakeHashKey(U32 level, U32 localX, U32 localY)
	{
		U32 hashValue = 0;
		HashCombine(hashValue, level);
		HashCombine(hashValue, localY);
		HashCombine(hashValue, localX);
		return hashValue;
	}

	template<typename T>
	inline bool QuadTreeNode<T>::IsSplit() const
	{
		return mChildren[0] != nullptr;
	}

	template<typename T>
	inline void QuadTreeNode<T>::Split(LinearAllocator& allocator)
	{
		using NodeType = QuadTreeNode<T>;

		Rect cellRect = GetRect();
		F32x2 cellSize = cellRect.GetSize();
		F32x2 cellSizeInv2 = { cellSize[0] * 0.5f, cellSize[1] * 0.5f };
		F32x2 ltPos = cellRect.GetPos();
		F32x2 centerPos = ltPos + cellSizeInv2;

		NodeType* nodes = (NodeType*)allocator.Allocate(sizeof(NodeType) * 4);

		mChildren[0] = new(nodes++) NodeType(Rect(ltPos, cellSizeInv2));
		mChildren[1] = new(nodes++) NodeType(Rect(F32x2(centerPos[0], ltPos[1]), cellSizeInv2));
		mChildren[2] = new(nodes++) NodeType(Rect(F32x2(ltPos[0], centerPos[1]), cellSizeInv2));
		mChildren[3] = new(nodes++) NodeType(Rect(centerPos, cellSizeInv2));

		static U32 localPosOffset[] = { 0,0, 1,0, 0,1, 1,1 };
		U32x2 currentLocalPos = GetLocalPos();
		for (U32 index = 0; index < 4; index++)
		{
			mChildren[index]->SetLocalPos({
				(currentLocalPos[0] << 1) + localPosOffset[index * 2 + 0],
				(currentLocalPos[1] << 1) + localPosOffset[index * 2 + 1],
			});

			mChildren[index]->SetDepth(mDepth + 1);
		}
	}

	template<typename T>
	inline void QuadTreeNode<T>::Clear()
	{
		std::fill(mChildren.begin(), mChildren.end(), nullptr);
	}


}