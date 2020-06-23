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

		void ConnectSignal(UI_EVENT_TYPE eventType, const SignalFunction& func);
		void DisconnectSignal(UI_EVENT_TYPE eventType, const SignalFunction& func);
		bool HasEventSignal(UI_EVENT_TYPE eventType) const;

		bool Fire(UI_EVENT_TYPE eventType, Dispatcher* target, const VariantArray& vairants);
		bool Fire(UI_EVENT_TYPE eventType, const VariantArray& variants);

	public:
		// 响应信号列表，目前只是简单的list
		template<typename T>
		struct SignalList
		{
			std::list<T> mList;

			// not use
			std::list<T> mPreList;
			std::list<T> mPostList;
		};

		template<typename T>
		struct SignalQueue
		{
			std::map<UI_EVENT_TYPE, SignalList<T>> mEventSignalMap;

			void ConnectSignal(UI_EVENT_TYPE eventType, const T& signal)
			{
				mEventSignalMap[eventType].mList.push_back(signal);
			}

			void DisconnectSignal(UI_EVENT_TYPE eventType, const T& signal)
			{
				mEventSignalMap[eventType].mList.remove_if(
					[&signal](T& element) { return signal.target_type() == element.target_type(); }
				);
			}

			bool HasConnectSignal(UI_EVENT_TYPE eventType)const
			{
				for (auto& kvp : mEventSignalMap)
				{
					auto& singalList = kvp.second;
					if (!singalList.mList.empty()) {
						return true;
					}
				}
				return false;
			}
		};

		SignalQueue< SignalFunction> mSignalQueue;
	};
}
}