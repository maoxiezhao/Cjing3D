#ifdef _WIN32

#include "gameWindowWin32.h"
#include "engine.h"
#include "helper\profiler.h"
#include "gui\guiStage.h"

#include <string>
#include <codecvt>
#include <locale>

namespace Cjing3D {

	void GameWindow::SetLoggerConsoleFontColor(ConsoleFontColor fontColor)
	{
		int color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
		switch (fontColor)
		{
		case Cjing3D::CONSOLE_FONT_BLUE:
			color = FOREGROUND_BLUE;
			break;
		case Cjing3D::CONSOLE_FONT_YELLOW:
			color = FOREGROUND_RED | FOREGROUND_GREEN;
			break;
		case Cjing3D::CONSOLE_FONT_GREEN:
			color = FOREGROUND_GREEN;
			break;
		case Cjing3D::CONSOLE_FONT_RED:
			color = FOREGROUND_RED;
			break;
		}

		HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(handle, FOREGROUND_INTENSITY | static_cast<WORD>(color));
	}

	void GameWindow::ShowMessageBox(const UTF8String& msg)
	{
		std::wstring msgWStr = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(msg.C_Str());
		MessageBoxW(NULL, LPCWSTR(msgWStr.c_str()), NULL, MB_OK);
	}

	GameWindowWin32::GameWindowWin32(const std::string& name, const I32x2& screenSize, bool fullScreen, DWORD style) :
		mTitleName(name),
		mScreenSize(screenSize),
		mFullScreen(fullScreen),
		mIsInitialized(false),
		mHinstance(GetModuleHandle(NULL)),
		mHwnd(NULL)
	{
		if (!InitializeWindows(name, screenSize[0], screenSize[1], style)) {
			Debug::Die("Failed to Initialize GameWindowWin32.");
		}
	}

	GameWindowWin32::~GameWindowWin32()
	{
		ShutdownWindow();
	}

	bool GameWindowWin32::Show()
	{
		ShowWindow(mHwnd, SW_SHOW);
		SetForegroundWindow(mHwnd);
		SetFocus(mHwnd);

		return true;
	}

	void GameWindowWin32::ShutDown()
	{
		ShutdownWindow();
	}

	bool GameWindowWin32::HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT & result)
	{
		for (auto hander : mHandlers)
		{
			if (hander->HandleMessage(hWnd, message, wParam, lParam, result))
				return true;
		}
		return false;
	}

	int GameWindowWin32::RunWindow(Engine& engine)
	{
		MSG msg;
		SecureZeroMemory(&msg, sizeof(msg));
		while (msg.message != WM_QUIT && !engine.GetIsExiting())
		{
			PROFILER_OPTICK_FRAME("mainThread");

			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				continue;
			}

			engine.Tick();
		}
		return static_cast<int> (msg.wParam);
	}

	bool GameWindowWin32::InitializeWindows(const std::string& name, int w, int h, DWORD style)
	{
		Logger::Info("[Video] Initialize GameWindowWin32 Size:" + std::to_string(w) + " " + std::to_string(h));
		std::wstring nameWStr = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(name.c_str());

		WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
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
		if (!IsFullScreen())
		{
			posX = (GetSystemMetrics(SM_CXSCREEN) - w) / 2;
			posY = (GetSystemMetrics(SM_CYSCREEN) - h) / 2;
		}

		mHwnd = CreateWindowExW(
			WS_EX_APPWINDOW,
			nameWStr.c_str(),
			nameWStr.c_str(),
			style,
			posX, posY,
			adjusted_rectangle.right - adjusted_rectangle.left,
			adjusted_rectangle.bottom - adjusted_rectangle.top,
			NULL, NULL,
			mHinstance,
			this);

		if (!mHwnd) {
			return false;
		}

		if (IsWindowUnicode(mHwnd) == 0) {
			return false;
		}

		SetDPI((I32)GetDpiForWindow(mHwnd));

		mIsInitialized = true;
		return true;
	}

	void GameWindowWin32::ShutdownWindow()
	{
		if (mIsInitialized)
		{
			mHandlers.clear();

			DestroyWindow(mHwnd);
			mHwnd = NULL;

			std::wstring nameWStr = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(mTitleName.c_str());
			UnregisterClass(nameWStr.c_str(), mHinstance);
			mHinstance = NULL;

			mIsInitialized = false;
		}
	}

	bool GameWindowWin32::IsWindowActive() const
	{
		return mHwnd == GetForegroundWindow();
	}

	GameWindowWin32 * GameWindowWin32::GetWindowCaller(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (WM_NCCREATE != message)
		{
			return reinterpret_cast<GameWindowWin32*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		}

		auto caller = reinterpret_cast<GameWindowWin32*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(caller));

		return caller;
	}

	LRESULT CALLBACK GameWindowWin32::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		auto caller = GetWindowCaller(hWnd, message, wParam, lParam);
		if (caller)
		{
			LRESULT result;
			if (caller->HandleMessage(hWnd, message, wParam, lParam, result))
				return result;
		}

		switch (message)
		{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_CLOSE:
			PostQuitMessage(0);
			break;
		case WM_CHAR:
		{
			std::wstring_convert<std::codecvt_utf8<wchar_t>> cvt;			
			auto& guiStage = GlobalGetSubSystem<GUIStage>();
			Gui::GUIInputEvent e = {};
			e.type = Gui::GUI_INPUT_EVENT_TYPE_INPUT_TEXT;
			e.inputText = cvt.to_bytes(static_cast<wchar_t>(wParam));
			guiStage.PushInputEvent(e);
		}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		return 0;
	}
}

#endif