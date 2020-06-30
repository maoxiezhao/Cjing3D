#pragma once

#include "gui\guiInclude.h"
#include "gui\guiCore\guiEvents.h"
#include "helper\variant.h"

#include <map>
#include <list>
#include <functional>

namespace Cjing3D {
namespace Gui {

	class Dispatcher;

	using SignalFunction = std::function<void(
		Dispatcher& dispatcher,
		bool& halt,
		bool& handle,
		const VariantArray& vairants
	)>;

	class Dispatcher
	{
	public:
		Dispatcher();
		~Dispatcher();

		enum SignalQueueposition
		{
			front_pre_child,
			back_pre_child,
			front_child,
			back_child,
			front_post_child,
			back_post_child
		};

		enum SingalQueueType
		{
			pre = 1,
			child = 2,
			post = 4
		};

		void ConnectSignal(UI_EVENT_TYPE eventType, const SignalFunction& func, SignalQueueposition pos = back_post_child);
		void DisconnectSignal(UI_EVENT_TYPE eventType, const SignalFunction& func, SignalQueueposition pos = back_post_child);
		bool HasEventSignal(UI_EVENT_TYPE eventType, U32 queueType = SingalQueueType::pre) const;

		bool Fire(UI_EVENT_TYPE eventType, Dispatcher* target, const VariantArray& vairants);
		bool Fire(UI_EVENT_TYPE eventType, const VariantArray& variants);

	public:

		// 响应信号列表，目前只是简单的list
		template<typename T>
		struct SignalList
		{
			std::list<T> mList;
			std::list<T> mPreList;
			std::list<T> mPostList;
		};

		template<typename T>
		struct SignalQueue
		{
			std::map<UI_EVENT_TYPE, SignalList<T>> mEventSignalMap;

			void ConnectSignal(UI_EVENT_TYPE eventType, const T& signal, SignalQueueposition pos = back_pre_child)
			{
				switch (pos)
				{
				case front_pre_child:
					mEventSignalMap[eventType].mPreList.push_front(signal);
					break;
				case back_pre_child:
					mEventSignalMap[eventType].mPreList.push_back(signal);
					break;
				case front_child:
					mEventSignalMap[eventType].mList.push_front(signal);
					break;
				case back_child:
					mEventSignalMap[eventType].mList.push_back(signal);
					break;
				case front_post_child:
					mEventSignalMap[eventType].mPostList.push_front(signal);
					break;
				case back_post_child:
					mEventSignalMap[eventType].mPostList.push_back(signal);
					break;
				}
			}

			void DisconnectSignal(UI_EVENT_TYPE eventType, const T& signal, SignalQueueposition pos = back_pre_child)
			{
				switch (pos)
				{
				case front_pre_child:
				case back_pre_child:
					mEventSignalMap[eventType].mPreList.remove_if(
						[&signal](T& element) { return signal.target_type() == element.target_type(); });
					break;
				case front_child:
				case back_child:
					mEventSignalMap[eventType].mList.remove_if(
						[&signal](T& element) { return signal.target_type() == element.target_type(); });
					break;
				case front_post_child:
				case back_post_child:
					mEventSignalMap[eventType].mPostList.remove_if(
						[&signal](T& element) { return signal.target_type() == element.target_type(); });
					break;
				}
			}

			bool HasConnectSignal(UI_EVENT_TYPE eventType, U32 queueType = SingalQueueType::pre)const
			{
				auto kvp = mEventSignalMap.find(eventType);
				if (kvp == mEventSignalMap.end()) {
					return false;
				}

				auto& singalList = kvp->second;
				if ((queueType & Dispatcher::pre) && !singalList.mPreList.empty()) {
					return true;
				}
				if ((queueType & Dispatcher::child) && !singalList.mList.empty()) {
					return true;
				}
				if ((queueType & Dispatcher::post) && !singalList.mPostList.empty()) {
					return true;
				}

				return false;
			}
		};

		SignalQueue< SignalFunction> mSignalQueue;
	};
}
}