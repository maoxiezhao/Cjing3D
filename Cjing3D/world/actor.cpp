#include "actor.h"
#include "world\component\renderable.h"

namespace Cjing3D
{
	Actor::Actor(SystemContext & gameContext):
		mGameContext(gameContext),
		mTransform(nullptr)
	{
	}

	Actor::~Actor()
	{
	}

	void Actor::Initialize()
	{
		ForEachComponent([](ComponentPtr& component) {
			component->Initialize();
		});
	}

	void Actor::Uninitialize()
	{
		ForEachComponent([](ComponentPtr& component) {
			component->Uninitialize();
		});
	}

	void Actor::Update()
	{
		ForEachComponent([](ComponentPtr& component) {
			component->Update();
		});
	}

	void Actor::Clone()
	{
	}

	void Actor::AddComponent(ComponentPtr component)
	{
		Component_Type type = component->GetType();
		if (type == ComponentType_Unknown) {
			return;
		}

		if (HasComponent(*component)) {
			return;
		}

		mComponentGUIDMap.insert(std::make_pair(component->GetGUID(), component));
		mComponents.push_back(component);
	}

	ComponentPtr Actor::AddComponent(Component_Type type)
	{
		ComponentPtr component = nullptr;
		switch (type)
		{
		case Cjing3D::ComponentType_Unknown:
			break;
		case Cjing3D::ComponentType_Renderable:
			component = AddComponent<Renderable>();
			break;
		default:
			break;
		}
		return component;
	}

	std::vector<ComponentPtr> Actor::GetComponents(Component_Type type)
	{
		std::vector<ComponentPtr> components;
		for (auto& component : mComponents)
		{
			if (component->GetType() == type) {
				components.push_back(component);
			}
		}
		return components;
	}

	bool Actor::HasComponent(Component & component)
	{
		return (mComponentGUIDMap.find(component.GetGUID()) != mComponentGUIDMap.end());
	}

	bool Actor::HasComponent(Component_Type type)
	{
		for (auto& component : mComponents)
		{
			if (component->GetType() == type) {
				return true;
			}
		}
		return false;
	}

	void Actor::RemoveComponent(U32 guid)
	{
		{
			auto it = mComponentGUIDMap.find(guid);
			if (it != mComponentGUIDMap.end()){
				mComponentGUIDMap.erase(it);
			}
		}
		{
			auto it = std::find_if(mComponents.begin(), mComponents.end(),
				[guid](auto& component) { return component->GetGUID() == guid; });

			if (it != mComponents.end()) {
				mComponents.erase(it);
			}
		}
	}

	void Actor::RemoveComponent(Component_Type type)
	{
		switch (type)
		{
		case Cjing3D::ComponentType_Unknown:
			break;
		case Cjing3D::ComponentType_Renderable:
			RemoveComponent<Renderable>();
			break;
		default:
			break;
		}
	}

	void Actor::AddChild(std::shared_ptr<Actor> actor)
	{
		if (actor == nullptr) {
			return;
		}

		if (actor->GetParent() != nullptr)
			actor->GetParent()->RemoveChild(actor);

		actor->mParent = this->shared_from_this();
		mChilds.push_back(actor);
	}

	void Actor::RemoveChild(std::shared_ptr<Actor> actor)
	{
		if (actor == nullptr || 
			actor->mParent.expired() || 
			actor->mParent.lock()->GetGUID() != this->GetGUID())
			return;

		actor->mParent.reset();

		const auto it = std::find(std::cbegin(mChilds), std::cend(mChilds), actor);
		if (it != mChilds.cend())
			mChilds.erase(it);
	}

	void Actor::RemoveAllChild()
	{
		ForEachChild([](ActorPtr& child) {
			child->mParent.reset();
		});
		mChilds.clear();
	}
}