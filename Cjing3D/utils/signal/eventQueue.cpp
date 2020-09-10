#include "eventQueue.h"

namespace Cjing3D
{
	EventQueue::EventQueue() :
		mSignal(std::make_shared<SignalType>())
	{
	}

	Connection EventQueue::Connect(EventHandler&& handler)
	{
		return mSignal->Connect(handler);
	}

	Connection EventQueue::Connect(const EventHandler& handler)
	{
		return mSignal->Connect(handler);
	}

	void EventQueue::Push(Event&& event)
	{
		GuardSpinLock lock(mSpinLock);
		mEvents.emplace_back(std::move(event));
	}

	void EventQueue::FireEvents()
	{
		std::vector<Event> currentEvents;
		{
			GuardSpinLock lock(mSpinLock);
			std::swap(currentEvents, mEvents);
		}

		for (const auto& event : currentEvents) {
			(*mSignal)(event);
		}
	}
}