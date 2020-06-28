#include "guiDispatcher.h"
#include "guiDispatcher_impl.h"

namespace Cjing3D {
namespace Gui {

	Dispatcher::Dispatcher()
	{
	}

	Dispatcher::~Dispatcher()
	{
	}

	void Dispatcher::ConnectSignal(UI_EVENT_TYPE eventType, const SignalFunction& func, SignalQueueposition pos)
	{
		mSignalQueue.ConnectSignal(eventType, func, pos);
	}

	void Dispatcher::DisconnectSignal(UI_EVENT_TYPE eventType, const SignalFunction& func, SignalQueueposition pos)
	{
		mSignalQueue.DisconnectSignal(eventType, func, pos);
	}

	bool Dispatcher::HasEventSignal(UI_EVENT_TYPE eventType, U32 queueType) const
	{
		return mSignalQueue.HasConnectSignal(eventType, queueType);
	}

	bool Dispatcher::Fire(UI_EVENT_TYPE eventType, Dispatcher* target, const VariantArray& vairants)
	{
		return FireEvent<SignalFunction>(eventType, this, target, vairants);
	}

	bool Dispatcher::Fire(UI_EVENT_TYPE eventType, const VariantArray& variants)
	{
		return false;
	}
}
}