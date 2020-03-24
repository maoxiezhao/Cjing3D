#pragma once

#include "utils\math.h"
#include "utils\allocator.h"

#include <set>
#include <functional>

namespace Cjing3D
{
	// 当前实现的四叉树，暂时不支持自动分区合并，只是用于地形的
	// LOD计算，保持四叉树的节点结构
	// TODO: 重构代码，支持完整四叉树

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
		QuadTree() {}
		~QuadTree() { Clear();}

		void Setup(const Rect& rect);
		void Clear();
		bool Insert(const T& element, const Rect& boundingBox);
		bool Remove(const T& element, const Rect& boundingBox);
		void SplitNode(QuadTreeNode<T>& node, LinearAllocator& allocator);

		QuadTreeNode<T>& GetRootNode() { return mRoot; }
		std::set<QuadTreeNode<T>*>& GetAllNodes() { return mAllNodes; }
		std::vector<T> GetAllElements();

		static const U32 MaxTreeNodeCount = 4;

	private:
		QuadTreeNode<T> mRoot;
		std::set<QuadTreeNode<T>*> mAllNodes;
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
		for (auto& node : mAllNodes) {
			node->Clear();
		}
		mAllNodes.clear();
		mRoot.Clear();
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
			mAllNodes.insert(child);
		}
	}

	template<typename T>
	inline std::vector<T> QuadTree<T>::GetAllElements()
	{
		std::vector<T> ret;

		return ret;
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