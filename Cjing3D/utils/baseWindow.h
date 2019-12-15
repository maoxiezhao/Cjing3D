#pragma once

#include "common\common.h"

#include <vector>

namespace Cjing3D {

	class WindowMessageHandler {
	public:
		WindowMessageHandler() = default;
		virtual ~WindowMessageHandler() = default;

		virtual bool HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT& result) = 0;
	};
	using WindowMessageHandlerPtr = std::shared_ptr<WindowMessageHandler>;

	class BaseWindow
	{
	public:
		BaseWindow();

		void AddMessageHandler(WindowMessageHandlerPtr handler);
		void RemoveHandler(WindowMessageHandlerPtr handler);

	protected:
		std::vector<WindowMessageHandlerPtr> mHandlers;
	};



}