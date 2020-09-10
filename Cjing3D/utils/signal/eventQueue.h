// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.
#pragma once

#include "utils\signal\event.h"
#include "utils\signal\signal.h"
#include "utils\thread\spinLock.h"

#include <functional>

namespace Cjing3D
{
	class EventQueue final
	{
	public:
		EventQueue();
		EventQueue(const EventQueue&) = delete;
		EventQueue(EventQueue&&) = delete;
		EventQueue& operator=(const EventQueue&) = delete;
		EventQueue& operator=(EventQueue&&) = delete;

		using EventHandler = std::function<void(const Event& event)>;
		Connection Connect(EventHandler&& handler);
		Connection Connect(const EventHandler& handler);
	
		void Push(Event&& event);

		template<typename T, typename... Args>
		void Push(Args&&... args)
		{
			Push(Event{ T{std::forward<Args>(args)...} });
		}

		void FireEvents();

	private:
		using SignalType = Signal<void(const Event&)>;
		std::shared_ptr<SignalType> mSignal;
		std::vector<Event> mEvents;
		SpinLock mSpinLock;
	};
}
