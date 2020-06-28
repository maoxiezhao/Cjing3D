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
		virtual bool IsWindowActive() const;

		I32 GetDPI()const { return mDPI; }
		void SetDPI(I32 dpi) { mDPI = dpi; }

	protected:
		std::vector<WindowMessageHandlerPtr> mHandlers;
		I32 mDPI = 96;
	};



}