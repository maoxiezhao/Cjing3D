#include "eventSystem.h"

namespace Cjing3D {

void EventManager::Register(EventType eventID, subscriber && func)
{
	mSubscribers[eventID].push_back(func);
}

void EventManager::RegisterWithMap(EventType eventID, subscriberWithMap && func)
{
	mSubscriberWithMaps[eventID].push_back(func);
}

void EventManager::Fire(EventType eventType, const Variant & data)
{
	if (mSubscribers.find(eventType) == mSubscribers.end())
		return;

	for (const auto& subscriber : mSubscribers[eventType])
		subscriber(data);
}

void EventManager::FireWithMap(EventType eventType, const VariantMap & data)
{
	if (mSubscriberWithMaps.find(eventType) == mSubscriberWithMaps.end())
		return;

	for (const auto& subscriber : mSubscriberWithMaps[eventType])
		subscriber(data);
}

void EventManager::Clear()
{
	mSubscribers.clear();
	mSubscriberWithMaps.clear();
}

EventManager::EventManager()
{
}

}
