#pragma once

#include <windows.h>
#include <string>
#include <vector>
#include <functional>

#include "common\common.h"
#include "utils\baseWindow.h"

namespace Cjing3D {

	class Window : public BaseWindow
	{
	public:
		static constexpr DWORD DefaultWindowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

		Window(const std::string& name, const I32x2& screenSize, bool fullScreen, DWORD style = DefaultWindowStyle);
		Window(const Window& rhs) = delete;
		Window& operator=(const Window& rhs) = delete;
		~Window();

		bool Show();
		void ShutDown();
		bool HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT&result);

		HINSTANCE GetInstance()const { return mHinstance; }
		HWND GetHwnd()const { return mHwnd; }
		bool IsFullScreen()const { return mFullScreen; }
		std::string GetTitleName()const { return mTitleName; }

		/** Wnd Proc Function */
		static Window* GetWindowCaller(HWND, UINT, WPARAM, LPARAM);
		static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	private:
		bool InitializeWindows(const std::string& name, int w, int h, DWORD style);
		void ShutdownWindow();

		std::string mTitleName;
		I32x2 mScreenSize;
		bool mFullScreen;
		bool mIsInitialized;

		HINSTANCE mHinstance;
		HWND mHwnd;
	};


}