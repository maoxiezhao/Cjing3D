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

	void Dispatcher::ConnectSignal(UI_EVENT_TYPE eventType, const SignalFunction& func)
	{
		mSignalQueue.ConnectSignal(eventType, func);
	}

	void Dispatcher::DisconnectSignal(UI_EVENT_TYPE eventType, const SignalFunction& func)
	{
		mSignalQueue.DisconnectSignal(eventType, func);
	}

	bool Dispatcher::HasEventSignal(UI_EVENT_TYPE eventType) const
	{
		return mSignalQueue.HasConnectSignal(eventType);
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