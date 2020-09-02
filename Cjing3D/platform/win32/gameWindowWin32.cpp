#ifdef _WIN32

#include "gameWindowWin32.h"
#include "platform\platform.h"
#include "helper\profiler.h"
#include "renderer\presentConfig.h"
#include "platform\systemEvent.h"

namespace Cjing3D::Win32 {

	namespace {

#ifndef RID_USAGE_GENERIC_MOUSE
#define RID_USAGE_GENERIC_MOUSE    ((USHORT)0x02)
#endif
#ifndef RID_USAGE_GENERIC_KEYBOARD
#define RID_USAGE_GENERIC_KEYBOARD ((USHORT)0x06)
#endif
		void RegisterInputDevice(HWND hWnd)
		{
			RAWINPUTDEVICE inputDevices[2] = {};

			// Register mouse:
			inputDevices[0].usUsagePage = 0x01;
			inputDevices[0].usUsage     = RID_USAGE_GENERIC_MOUSE;
			inputDevices[0].dwFlags = 0;
			inputDevices[0].hwndTarget = 0;

			// Register keyboard:
			inputDevices[1].usUsagePage = 0x01;
			inputDevices[1].usUsage = RID_USAGE_GENERIC_KEYBOARD;
			inputDevices[1].dwFlags = 0;
			inputDevices[1].hwndTarget = 0;

			if (RegisterRawInputDevices(
				inputDevices, 
				ARRAYSIZE(inputDevices),
				sizeof(inputDevices[0])) == FALSE)
			{
				Debug::Die("Failed to Win32::InputDevice");
			}
		}
	}

	GameWindowWin32::GameWindowWin32(
			HINSTANCE hInstance, 
			const UTF8String& titleName, 
			const std::shared_ptr<EventQueue>& eventQueue,
			const PresentConfig& config) :
		mIsInitialized(false),
		mHinstance(hInstance),
		mHwnd(NULL),
		mTitleName(titleName),
		mScreenSize(config.mScreenSize),
		mEventQueue(eventQueue),
		mIsFullScreen(config.mIsFullScreen)
	{
		
		std::wstring nameWStr = Platform::StringToWString(mTitleName.String());

		LONG adjustedWidth = static_cast<LONG>(mScreenSize[0]);
		LONG adjustedHeight = static_cast<LONG>(mScreenSize[1]);
		DWORD windowStyle = 0;
		DWORD windowStyleEx = 0;

		if (mIsFullScreen)
		{
			windowStyle |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
			windowStyleEx |= WS_EX_TOPMOST;
		}
		else
		{
			windowStyle |= WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

			RECT rectangle = { 0, 0, static_cast<LONG>(mScreenSize[0]), static_cast<LONG>(mScreenSize[1])};
			AdjustWindowRect(&rectangle, windowStyle, FALSE);

			adjustedWidth  = rectangle.right - rectangle.left;
			adjustedHeight = rectangle.bottom - rectangle.top;
		}

		WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = GameWindowWin32::WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = mHinstance;
		wcex.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = nullptr;
		wcex.lpszClassName = nameWStr.c_str();
		wcex.hIconSm = LoadIcon(nullptr, IDI_WINLOGO);

		if (!RegisterClassEx(&wcex)) {
			Debug::Die("Failed to RegisterClassEx");
		}
			
		mHwnd = CreateWindowExW(
			windowStyleEx,
			nameWStr.c_str(),
			nameWStr.c_str(),
			windowStyle,
			CW_USEDEFAULT, CW_USEDEFAULT,
			adjustedWidth, adjustedHeight,
			NULL, NULL,
			mHinstance,
			this);
		if (!mHwnd) {
			Debug::Die("Failed to CreateWindowExW");
		}

		if (FAILED(::CoInitializeEx(nullptr, COINIT_MULTITHREADED | COINIT_DISABLE_OLE1DDE))) {
			Debug::Die("Failed to CoInitializeEx");
		}

		if (IsWindowUnicode(mHwnd) == 0) {
			Debug::Die("Window must is unicode");
		}

		ShowWindow(mHwnd, SW_SHOW);
		SetForegroundWindow(mHwnd);
		SetFocus(mHwnd);
		RegisterInputDevice(mHwnd);

		mDPI = (I32)GetDpiForWindow(mHwnd);

		POINT point = { 0, 0 };
		if (::ClientToScreen(mHwnd, &point))
		{
			mScreenPos[0] = static_cast<std::int32_t>(point.x);
			mScreenPos[1] = static_cast<std::int32_t>(point.y);
		}

		Logger::Info("[Video] Initialize GameWindowWin32 Size:" + std::to_string(adjustedWidth) + " " + std::to_string(adjustedHeight));
		mIsInitialized = true;
	}

	GameWindowWin32::~GameWindowWin32()
	{
		ShutDown();
	}

	void GameWindowWin32::ShutDown()
	{
		if (mIsInitialized)
		{
			mHandlers.clear();

			DestroyWindow(mHwnd);
			mHwnd = NULL;

			std::wstring nameWStr = Platform::StringToWString(mTitleName.String());
			UnregisterClass(nameWStr.c_str(), mHinstance);
			mHinstance = NULL;

			::CoUninitialize();
			mIsInitialized = false;
		}
	}

	bool GameWindowWin32::HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT & result)
	{
		WindowMessageData messageData;
		messageData.handle = GetHwnd();
		messageData.message = message;
		messageData.wparam  = wParam;
		messageData.lparam  = lParam;

		for (auto handler : mHandlers)
		{
			if (handler != nullptr && handler(messageData)) {
				return true;
			}
		}
		return false;
	}

	bool GameWindowWin32::Tick()
	{
		MSG msg;
		SecureZeroMemory(&msg, sizeof(msg));

		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		return true;
	}

	void GameWindowWin32::AddMessageHandler(WindowMessageHandler handler)
	{
		mHandlers.push_back(handler);
	}

	bool GameWindowWin32::IsWindowActive() const
	{
		return ::GetForegroundWindow() == mHwnd;
	}

	bool GameWindowWin32::IsFullScreen() const
	{
		return mIsFullScreen;
	}

	UTF8String GameWindowWin32::GetWindowTitle() const
	{
		return mTitleName;
	}

	void GameWindowWin32::SetClientbounds(const RectInt& rect)
	{
		if (mIsFullScreen) {
			return;
		}

		DWORD const dwStyle = static_cast<DWORD>(::GetWindowLong(mHwnd, GWL_STYLE));
		RECT rectangle = {
			0,
			0,
			static_cast<LONG>(rect.mRight - rect.mLeft),
			static_cast<LONG>(rect.mBottom - rect.mTop)
		};
		AdjustWindowRect(&rectangle, dwStyle, FALSE);

		int const adjustedWidth = static_cast<int>(rectangle.right - rectangle.left);
		int const adjustedHeight = static_cast<int>(rectangle.bottom - rectangle.top);
		UINT const flags = SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE;

		if (::SetWindowPos(mHwnd, 0, 0, 0, adjustedWidth, adjustedHeight, flags) == 0) {
			return;
		}

		mScreenSize[0] = rect.mRight - rect.mLeft;
		mScreenSize[1] = rect.mBottom - rect.mTop;
	}

	I32 GameWindowWin32::GetDPI() const
	{
		return mDPI;
	}

	void GameWindowWin32::SetWindowTitle(const UTF8String& titleName)
	{
		mTitleName = titleName;
		SetWindowText(mHwnd, Platform::StringToWString(titleName.String()).c_str());
	}

	bool GameWindowWin32::IsMouseCursorVisible() const
	{
		return mIsMouseCursorVisible;
	}

	void GameWindowWin32::SetMouseCursorVisible(bool visible)
	{
		if (mIsMouseCursorVisible != mIsMouseCursorVisible)
		{
			mIsMouseCursorVisible = visible;

			if (mIsMouseCursorVisible) {
				::SetCursor(LoadCursor(nullptr, IDC_ARROW));
			}
			else {
				::SetCursor(nullptr);
			}
		}
	}

	RectInt GameWindowWin32::GetClientBounds() const
	{
		return { 0, 0, mScreenSize[0], mScreenSize[1] };
	}

	GameWindowWin32 * GameWindowWin32::GetWindowCaller(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (WM_NCCREATE != message) {
			return reinterpret_cast<GameWindowWin32*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		}

		auto caller = reinterpret_cast<GameWindowWin32*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(caller));
		return caller;
	}

	LRESULT CALLBACK GameWindowWin32::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		LRESULT result;
		auto window = GetWindowCaller(hWnd, message, wParam, lParam);
		if (window != nullptr && window->HandleMessage(hWnd, message, wParam, lParam, result)) {
			return result;
		}

		switch (message)
		{
		case WM_DESTROY:
			::PostQuitMessage(0);
			if (window != nullptr) {
				window->mEventQueue->Push<WindowCloseEvent>();
			}
			return 0;
		case WM_CLOSE:
			if (window != nullptr) {
				window->mEventQueue->Push<WindowCloseEvent>();
			}
			return 0;
		case WM_CHAR:
			if (window != nullptr) 
			{
				std::wstring text;
				text += static_cast<wchar_t>(wParam);
				window->mEventQueue->Push<InputTextEvent>(Platform::WStringToString(text));
			}
			return 0;
		case WM_MOVE: 
			if (window != nullptr) 
			{
				window->mScreenPos[0] = static_cast<I32>(LOWORD(lParam));
				window->mScreenPos[1] = static_cast<I32>(HIWORD(lParam));
			}
			break;
		case WM_INPUT: 
			{
				if (window != nullptr)
				{
					static RAWINPUT raw;
					U32 rawSize = sizeof(raw);
					GetRawInputData((HRAWINPUT)lParam, RID_INPUT, &raw, &rawSize, sizeof(RAWINPUTHEADER));
			
					if (raw.header.dwType == RIM_TYPEMOUSE) {
						window->mEventQueue->Push<RAWMOUSE>(raw.data.mouse);
					}
					else if (raw.header.dwType == RIM_TYPEKEYBOARD) {
						window->mEventQueue->Push<RAWKEYBOARD>(raw.data.keyboard);
					}
				}
			}
			break;
		case WM_SIZE:
			{
				if (window != nullptr)
				{
					U32 width = static_cast<U32>(lParam & 0xffff);
					U32 height = static_cast<U32>((lParam >> 16) & 0xffff);
					window->mEventQueue->Push<ViewResizeEvent>(width, height);
				}
			}
			return true;
		default:
			break;
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

#endif