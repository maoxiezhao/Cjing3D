#pragma once

#ifdef _WIN32
#include "platform\gameWindow.h"

#define NOMINMAX
#include <windows.h>
#include <string>
#include <vector>
#include <functional>

namespace Cjing3D {

	class GameWindowWin32 : public GameWindow
	{
	public:
		static constexpr DWORD DefaultWindowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

		GameWindowWin32(const std::string& titleName, const I32x2& screenSize, bool fullScreen, DWORD style = DefaultWindowStyle);
		GameWindowWin32(const GameWindowWin32& rhs) = delete;
		GameWindowWin32& operator=(const GameWindowWin32& rhs) = delete;
		~GameWindowWin32();

		bool Show();
		void ShutDown();
		bool HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT&result);
		int  RunWindow(Engine& engine)override;

		HINSTANCE GetInstance()const { return mHinstance; }
		HWND GetHwnd()const { return mHwnd; }
	
		virtual void SetWindowSize(const I32x2 size);
		virtual void SetIsWindowFullScreen(bool isFullScreen);
		virtual void SetWindowTitle(const UTF8String& titleName);
		virtual bool IsWindowActive() const;

		/** Wnd Proc Function */
		static GameWindowWin32* GetWindowCaller(HWND, UINT, WPARAM, LPARAM);
		static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	private:
		bool InitializeWindows(const std::string& name, int w, int h, DWORD style);
		void ShutdownWindow();

		bool mIsInitialized;
		HINSTANCE mHinstance;
		HWND mHwnd;
		DWORD mStyle;
	};
}

#endif