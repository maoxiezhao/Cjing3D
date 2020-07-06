#pragma once

#include "gui\guiWidgets\widgets.h"

#include <map>
#include <list>
#include <functional>

namespace Cjing3D {
namespace Gui {

	namespace Implementation
	{
		#define IMPLEMENT_EVENT_SIGNAL(FUNCTION, QUEUE)						\
		template<typename F>                                        \
		inline std::enable_if_t<std::is_same<FUNCTION, F>::value, Dispatcher::SignalList<FUNCTION>> & \
			GetEventSignalList(Dispatcher& dispatcher, UI_EVENT_TYPE eventType) {             \
			return dispatcher.QUEUE.mEventSignalMap[eventType];                               \
		}

		IMPLEMENT_EVENT_SIGNAL(SignalFunction, mSignalQueue);

		#undef IMPLEMENT_EVENT_SIGNAL

		template<typename T, typename... F>
		inline bool FireEvent(const UI_EVENT_TYPE eventType, std::vector<std::pair<Dispatcher*, UI_EVENT_TYPE>>& eventChain, Dispatcher& dispatcher, Dispatcher& target,  F&&... params)
		{
			// handle pre signal
			std::vector<std::pair<Dispatcher*, UI_EVENT_TYPE> > reverseEventChain = eventChain;
			std::reverse(reverseEventChain.begin(), reverseEventChain.end());

			for (auto& pair : reverseEventChain)
			{
				Dispatcher* widget = pair.first;
				UI_EVENT_TYPE eventType = pair.second;
				if (widget != nullptr && widget->HasEventSignal(eventType, Dispatcher::pre))
				{
					bool halt = false;
					bool handle = false;

					Dispatcher::SignalList<T>& signalList = Implementation::GetEventSignalList<T>(*widget, eventType);
					for (auto& func : signalList.mPreList)
					{
						func(dispatcher, halt, handle, std::forward<F>(params)...);
						if (halt) {
							break;
						}
					}

					if (handle) {
						return true;
					}
				}
			}

			// handle current signal
			if (target.HasEventSignal(eventType, Dispatcher::child))
			{
				bool halt = false;
				bool handle = false;

				Dispatcher::SignalList<T>& signalList = Implementation::GetEventSignalList<T>(target, eventType);
				for (auto& func : signalList.mList)
				{
					func(dispatcher, halt, handle, std::forward<F>(params)...);
					if (halt) {
						break;
					}
				}

				if (handle) {
					return true;
				}
			}

			// TODO: handle post signal
			for (auto& pair : eventChain)
			{
				Dispatcher* widget = pair.first;
				UI_EVENT_TYPE eventType = pair.second;
				if (widget != nullptr && widget->HasEventSignal(eventType, Dispatcher::post))
				{
					bool halt = false;
					bool handle = false;

					Dispatcher::SignalList<T>& signalList = Implementation::GetEventSignalList<T>(*widget, eventType);
					for (auto& func : signalList.mPostList)
					{
						func(dispatcher, halt, handle, std::forward<F>(params)...);
						if (halt) {
							break;
						}
					}

					if (handle) {
						return true;
					}
				}
			}

			return false;
		}

		inline void BuildEventChain(UI_EVENT_TYPE eventType, Widget* dispatcher, Widget* target, std::vector<std::pair<Dispatcher*, UI_EVENT_TYPE>>& eventChain)
		{
			if (dispatcher == nullptr || target == nullptr) {
				return;
			}

			Widget* widget = target;
			while (widget != nullptr)
			{
				// 当存在preSignal和postSignal时都加入队列
				if (widget->HasEventSignal(eventType, Dispatcher::pre | Dispatcher::post)) {
					eventChain.emplace_back(widget, eventType);
				}

				if (widget == dispatcher) {
					break;
				}

				widget = widget->GetParent();
			}
		}
	}

	template<typename T, typename... F>
	inline bool FireEvent(const UI_EVENT_TYPE eventType, Dispatcher* dispatcher, Dispatcher* target, F&&... params)
	{
		if (dispatcher == nullptr || target == nullptr) {
			return false;
		}

		Widget* dispatcherW = static_cast<Widget*>(dispatcher);
		Widget* targetW = static_cast<Widget*>(target);

		std::vector<std::pair<Dispatcher*, UI_EVENT_TYPE>> eventChain;
		Implementation::BuildEventChain(eventType, dispatcherW, targetW, eventChain);

		return Implementation::FireEvent<T>(eventType, eventChain, *dispatcher, *target, std::forward<F>(params)...);
	}

}
}