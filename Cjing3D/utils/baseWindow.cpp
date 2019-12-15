#include "baseWindow.h"

namespace Cjing3D {

	BaseWindow::BaseWindow()
	{
	}

	void BaseWindow::AddMessageHandler(WindowMessageHandlerPtr handler)
	{
		mHandlers.push_back(handler);
	}

	void BaseWindow::RemoveHandler(WindowMessageHandlerPtr handler)
	{
		mHandlers.erase(std::remove(mHandlers.begin(), mHandlers.end(), handler), mHandlers.end());
	}
}