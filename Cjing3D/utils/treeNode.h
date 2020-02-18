#pragma once

#include "helper\stringID.h"

#include <memory>
#include <deque>
#include <map>
#include <list>

namespace Cjing3D
{
	// 树形节点结构，提供children和parent
	template<typename T>
	class TreeNode : public std::enable_shared_from_this<TreeNode<T>>
	{
	public:
		using Node = T;
		using NodePtr = std::shared_ptr<Node>;
		using ChildrenType = std::list<NodePtr>;
		using ChildrenIter = typename ChildrenType::iterator;

	protected:
		StringID mName;
		T* mParent = nullptr;
		ChildrenType mChildren;
		std::map<StringID, NodePtr> mChildrenNameMap;

	public:
		TreeNode(const StringID& name) : mName(name) {};
		~TreeNode() 
		{
			for (auto& child : mChildren) {
				child->SetParent(nullptr);
			}
			mChildren.clear();
			mChildrenNameMap.clear();
		}

		void Add(NodePtr node)
		{
			if (HaveChild(node)) {
				return;
			}

			StringID name = node->GetName();
			if (name != StringID::EMPTY) {
				mChildrenNameMap[name] = node;
			}

			mChildren.push_back(node);

			node->resetParent();
			node->SetParent((T*)this);
			OnChildAdded(node);
		}

		void Remove(const StringID& name)
		{
			NodePtr node = Find(name);
			if (node == nullptr) {
				return;
			}

			mChildrenNameMap.erase(name);

			auto iter = std::find(mChildren.begin(), mChildren.end(), node);
			if (iter != mChildren.end()) {
				mChildren.erase(iter);
			}

			OnChildRemoved(node);
			node->SetParent(nullptr);
		}

		void Remove(NodePtr node)
		{
			if (!HaveChild(node)) {
				return;
			}

			StringID name = node->GetName();
			if (name != StringID::EMPTY) {
				mChildrenNameMap.erase(name);
			}

			auto iter = std::find(mChildren.begin(), mChildren.end(), node);
			if (iter != mChildren.end()) {
				mChildren.erase(iter);
			}

			OnChildRemoved(node);
			node->SetParent(nullptr);
		}

		void ClearChildren()
		{
			for (auto& child : mChildren) {
				child->SetParent(nullptr);
			}

			mChildren.clear();
			mChildrenNameMap.clear();
		}

		bool HaveChild(NodePtr node)const
		{
			StringID name = node->GetName();
			if (name != StringID::EMPTY) {
				return HaveChild(name);
			}

			auto iter = std::find(mChildren.begin(), mChildren.end(), node);
			return iter != mChildren.end();
		}

		bool HaveChild(const StringID& name)const
		{
			return mChildrenNameMap.find(name) != mChildrenNameMap.end();
		}

		NodePtr Find(const StringID& name)
		{
			auto it = mChildrenNameMap.find(name);
			if (it == mChildrenNameMap.end()) {
				return nullptr;
			}
			return it->second;
		}

		StringID GetName()const { return mName; }
		void SetName(const StringID& name) { mName = name; }
		NodePtr GetNodePtr() { return std::static_pointer_cast<T>( this->shared_from_this());}
		T* GetParent() { return mParent; }
		const T* GetParent()const { return mParent; }

		void resetParent()
		{
			if (mParent != nullptr){
				mParent->Remove(GetNodePtr());
			}
		}

		void SetParent(T* node)
		{
			T* oldParent = mParent;
			mParent = node;
			OnParentChanged(oldParent);
		}

	protected:
		virtual void OnParentChanged(T* old_parent) {}
		virtual void OnChildAdded(NodePtr& node) {}
		virtual void OnChildRemoved(NodePtr& node) {}
	};
}