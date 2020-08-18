#ifdef _WIN32

#include "gameWindowWin32.h"
#include "engine.h"
#include "helper\profiler.h"
#include "gui\guiStage.h"

#include <string>
#include <ShlObj.h>

namespace Cjing3D {

	std::string GameWindow::WStringToString(const std::wstring& wstr)
	{
		if (wstr.empty()) return std::string();
		int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
		std::string strTo(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
		return strTo;
	}

	// Convert an UTF8 string to a wide Unicode String
	std::wstring GameWindow::StringToWString(const std::string& str)
	{
		if (str.empty()) return std::wstring();
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
		std::wstring wstrTo(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
		return wstrTo;
	}

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

	void GameWindow::LoadFileFromOpenWindow(const char* fileFilter, std::function<void(const std::string&)> callback)
	{
		char szFile[256] = { '\0' };

		OPENFILENAMEA ofn;
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = nullptr;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = fileFilter;
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = nullptr;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = nullptr;

		// If you change folder in the dialog it will change the current folder for your process without OFN_NOCHANGEDIR;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetOpenFileNameA(&ofn))
		{
			if (callback != nullptr) {
				callback(ofn.lpstrFile);
			}
		}
	}

	void GameWindow::SaveFileToOpenWindow(const char* fileFilter, std::function<void(const std::string&)> callback)
	{
		char szFile[256] = { '\0' };

		OPENFILENAMEA ofn;
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = nullptr;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = fileFilter;
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = nullptr;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = nullptr;
		ofn.Flags = OFN_OVERWRITEPROMPT;

		if (GetSaveFileNameA(&ofn))
		{
			if (callback != nullptr) {
				callback(ofn.lpstrFile);
			}
		}
	}

	void GameWindow::ShowBrowseForFolder(const char* title, std::function<void(const std::string&)> callback)
	{
		std::wstring nameWStr = StringToWString(title);
		wchar_t szBuffer[256] = { '\0' };
		BROWSEINFO bi;
		ZeroMemory(&bi, sizeof(BROWSEINFO));
		bi.hwndOwner = NULL;
		bi.pszDisplayName = szBuffer;
		bi.lpszTitle = nameWStr.c_str();
		bi.ulFlags = BIF_RETURNFSANCESTORS;

		LPITEMIDLIST idl = SHBrowseForFolder(&bi);
		if (NULL == idl) {
			return;
		}

		SHGetPathFromIDList(idl, szBuffer);
		callback(WStringToString(nameWStr));
	}

	void GameWindow::ShowMessageBox(const UTF8String& msg)
	{
		std::wstring msgWStr = StringToWString(msg.C_Str());
		MessageBoxW(NULL, LPCWSTR(msgWStr.c_str()), NULL, MB_OK);
	}

	GameWindowWin32::GameWindowWin32(const std::string& titleName, const I32x2& screenSize, bool fullScreen, DWORD style) :
		mIsInitialized(false),
		mHinstance(GetModuleHandle(NULL)),
		mHwnd(NULL),
		mStyle(style)
	{
		mTitleName = titleName;
		mScreenSize = screenSize;
		mFullScreen = fullScreen;

		if (!InitializeWindows(titleName, screenSize[0], screenSize[1], style)) {
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
		std::wstring nameWStr = StringToWString(name);

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

			std::wstring nameWStr = StringToWString(mTitleName);
			UnregisterClass(nameWStr.c_str(), mHinstance);
			mHinstance = NULL;

			mIsInitialized = false;
		}
	}

	void GameWindowWin32::SetWindowSize(const I32x2 size)
	{
		if (size == mScreenSize) {
			return;
		}
		mScreenSize = size;

		////////////////////////////////////////////////////////////
		// TEMP
		const RECT rectangle = {
			0,
			0,
			static_cast<LONG>(mScreenSize[0]),
			static_cast<LONG>(mScreenSize[1])
		};
		auto adjusted_rectangle = rectangle;
		AdjustWindowRect(&adjusted_rectangle, mStyle, FALSE);

		int posX = 0, posY = 0;
		if (!IsFullScreen())
		{
			posX = (GetSystemMetrics(SM_CXSCREEN) - mScreenSize[0]) / 2;
			posY = (GetSystemMetrics(SM_CYSCREEN) - mScreenSize[1]) / 2;
		}

		MoveWindow(
			mHwnd,
			posX, posY,
			adjusted_rectangle.right - adjusted_rectangle.left,
			adjusted_rectangle.bottom - adjusted_rectangle.top,
			false
		);
		////////////////////////////////////////////////////////////
	}

	void GameWindowWin32::SetIsWindowFullScreen(bool isFullScreen)
	{
		if (mFullScreen == isFullScreen) {
			return;
		}
		mFullScreen = isFullScreen;
	}

	void GameWindowWin32::SetWindowTitle(const UTF8String& titleName)
	{
		SetWindowText(mHwnd, StringToWString(titleName.String()).c_str());
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
			std::wstring wstring;
			wstring += static_cast<wchar_t>(wParam);

			auto& guiStage = GlobalGetSubSystem<GUIStage>();
			Gui::GUIInputEvent e = {};
			e.type = Gui::GUI_INPUT_EVENT_TYPE_INPUT_TEXT;
			e.inputText = WStringToString(wstring);
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