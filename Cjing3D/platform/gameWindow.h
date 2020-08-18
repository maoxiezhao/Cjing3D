#pragma once

#include "common\common.h"
#include "utils\string\utf8String.h"
 
#include <vector>
#include <functional>

namespace Cjing3D {

#define DEFAULT_GAME_WINDOW_WIDTH  1440
#define DEFAULT_GAME_WINDOW_HEIGHT 800
#define DEFAULT_GAME_WINDOW_DPI 96

	class Engine;

	class WindowMessageHandler {
	public:
		WindowMessageHandler() = default;
		virtual ~WindowMessageHandler() = default;

		virtual bool HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT& result) = 0;
	};
	using WindowMessageHandlerPtr = std::shared_ptr<WindowMessageHandler>;

	// console
	enum ConsoleFontColor
	{
		CONSOLE_FONT_WHITE,
		CONSOLE_FONT_BLUE,
		CONSOLE_FONT_YELLOW,
		CONSOLE_FONT_GREEN,
		CONSOLE_FONT_RED
	};

	class GameWindow
	{
	public:
		GameWindow() = default;

		// string
		static std::string WStringToString(const std::wstring& wstr);
		static std::wstring StringToWString(const std::string& str);

		// window
		void AddMessageHandler(WindowMessageHandlerPtr handler);
		void RemoveHandler(WindowMessageHandlerPtr handler);

		virtual bool IsWindowActive() const;
		virtual int RunWindow(Engine& engine);
		virtual void SetWindowTitle(const UTF8String& titleName);
		virtual void SetWindowSize(const I32x2 size);
		virtual void SetIsWindowFullScreen(bool isFullScreen);

		// console
		static void SetLoggerConsoleFontColor(ConsoleFontColor fontColor);
		static void ShowMessageBox(const UTF8String& msg);
		static void LoadFileFromOpenWindow(const char* fileFilter, std::function<void(const std::string&)> callback);
		static void SaveFileToOpenWindow(const char* fileFilter, std::function<void(const std::string&)> callback);
		static void ShowBrowseForFolder(const char* title, std::function<void(const std::string&)> callback);

		I32 GetDPI()const { return mDPI; }
		void SetDPI(I32 dpi) { mDPI = dpi; }
		bool IsFullScreen()const { return mFullScreen; }
		std::string GetTitleName()const { return mTitleName; }
		I32x2 GetScreenSize()const { return mScreenSize; }

	protected:
		std::vector<WindowMessageHandlerPtr> mHandlers;
		I32 mDPI = DEFAULT_GAME_WINDOW_DPI;
		std::string mTitleName;
		I32x2 mScreenSize = I32x2(0, 0);
		bool mFullScreen = false;
	};



}