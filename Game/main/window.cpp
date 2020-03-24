#include "window.h"

namespace Cjing3D {

	Window::Window(const std::string& name, const I32x2& screenSize, bool fullScreen, DWORD style) :
		mTitleName(name),
		mScreenSize(screenSize),
		mFullScreen(fullScreen),
		mIsInitialized(false),
		mHinstance(GetModuleHandle(NULL)),
		mHwnd(NULL)
	{
		if (!InitializeWindows(name, screenSize[0], screenSize[1], style))
			Debug::Die("Failed to Initialize Window.");
	}

	Window::~Window()
	{
		ShutdownWindow();
	}

	bool Window::Show()
	{
		ShowWindow(mHwnd, SW_SHOW);
		SetForegroundWindow(mHwnd);
		SetFocus(mHwnd);

		return true;
	}

	void Window::ShutDown()
	{
		ShutdownWindow();
	}

	bool Window::HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT & result)
	{
		for (auto hander : mHandlers)
		{
			if (hander->HandleMessage(hWnd, message, wParam, lParam, result))
				return true;
		}
		return false;
	}

	bool Window::InitializeWindows(const std::string& name, int w, int h, DWORD style)
	{
		Logger::Info("[Video] Initialize Window Size:" + std::to_string(w) + " " + std::to_string(h));
		WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wcex.lpfnWndProc = Window::WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = mHinstance;
		wcex.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = nullptr;
		wcex.lpszClassName = name.c_str();
		wcex.hIconSm = LoadIcon(nullptr, IDI_WINLOGO);

		if (!RegisterClassEx(&wcex))
			return false;

		const RECT rectangle = {
			0,
			0,
			static_cast<LONG>(w),
			static_cast<LONG>(h)
		};
		auto adjusted_rectangle = rectangle;
		AdjustWindowRect(&adjusted_rectangle, style, FALSE);

		int posX = 0, posY = 0;
		if (IsFullScreen())
		{
		}
		else
		{
			posX = (GetSystemMetrics(SM_CXSCREEN) - w) / 2;
			posY = (GetSystemMetrics(SM_CYSCREEN) - h) / 2;
		}

		mHwnd = CreateWindowEx(
			WS_EX_APPWINDOW,
			name.c_str(), 
			name.c_str(),
			style,
			posX, posY,
			adjusted_rectangle.right - adjusted_rectangle.left,
			adjusted_rectangle.bottom - adjusted_rectangle.top,
			NULL, NULL,
			mHinstance,
			this);

		if (!mHwnd)
			return false;

		mIsInitialized = true;
		return true;
	}

	void Window::ShutdownWindow()
	{
		if (mIsInitialized)
		{
			mHandlers.clear();

			DestroyWindow(mHwnd);
			mHwnd = NULL;
			UnregisterClass(mTitleName.c_str(), mHinstance);
			mHinstance = NULL;

			mIsInitialized = false;
		}
	}

	bool Window::IsWindowActive() const
	{
		return mHwnd == GetForegroundWindow();
	}

	Window * Window::GetWindowCaller(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (WM_NCCREATE != message)
		{
			return reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		}

		auto caller = reinterpret_cast<Window*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(caller));

		return caller;
	}

	LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		{
			auto caller = GetWindowCaller(hWnd, message, wParam, lParam);
			if (caller)
			{
				LRESULT result;
				if (caller->HandleMessage(hWnd, message, wParam, lParam, result))
					return result;
			}
		}

		switch (message)
		{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_CLOSE:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		return 0;
	}


}
