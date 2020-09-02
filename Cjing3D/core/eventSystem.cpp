#include "eventSystem.h"

namespace Cjing3D
{
namespace EventSystem
{
	struct EventManager
	{
		using Subscriber = Signal<void(const VariantArray& variants)>;

		SpinLock mSpinLock;
		std::unordered_map<EventType, Subscriber> mSubscribers;
		std::unordered_map<EventType, Subscriber> mSubscribersOnce;
	};
	std::shared_ptr<EventManager> manager = std::make_shared<EventManager>();

	Connection Register(EventType eventID, CallbackFunc func)
	{
		Connection connection;
		{
			GuardSpinLock lock(manager->mSpinLock);
			connection = manager->mSubscribers[eventID].Connect(func);
		}	
		return connection;
	}

	void RegisterOnce(EventType eventID, CallbackFunc func)
	{
		GuardSpinLock lock(manager->mSpinLock);
		manager->mSubscribersOnce[eventID].Connect(func);
	}

	void Fire(EventType eventID, const VariantArray& variants)
	{
		GuardSpinLock lock(manager->mSpinLock);
		{
			// call once
			{
				auto it = manager->mSubscribersOnce.find(eventID);
				if (it != manager->mSubscribersOnce.end())
				{
					it->second(variants);
					it->second.ClearAll();
				}
			}
			// call until deleted
			{
				auto it = manager->mSubscribers.find(eventID);
				if (it != manager->mSubscribers.end())
				{
					it->second(variants);
				}
			}
		}
	}
}
}
