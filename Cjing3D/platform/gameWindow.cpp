#include "gameWindow.h"

namespace Cjing3D {

	void GameWindow::AddMessageHandler(WindowMessageHandlerPtr handler)
	{
		mHandlers.push_back(handler);
	}

	void GameWindow::RemoveHandler(WindowMessageHandlerPtr handler)
	{
		mHandlers.erase(std::remove(mHandlers.begin(), mHandlers.end(), handler), mHandlers.end());
	}

	bool GameWindow::IsWindowActive() const
	{
		return true;
	}

	int GameWindow::RunWindow(Engine& engine)
	{
		return 0;
	}

	void GameWindow::SetWindowTitle(const UTF8String& titleName)
	{
	}
}