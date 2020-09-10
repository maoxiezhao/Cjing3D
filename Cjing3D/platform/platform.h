#pragma once

#include "common\common.h"
#include "utils\string\utf8String.h"

namespace Cjing3D {

#define DEFAULT_GAME_WINDOW_WIDTH  1440
#define DEFAULT_GAME_WINDOW_HEIGHT 800
#define DEFAULT_GAME_WINDOW_DPI 96

	// console
	enum ConsoleFontColor
	{
		CONSOLE_FONT_WHITE,
		CONSOLE_FONT_BLUE,
		CONSOLE_FONT_YELLOW,
		CONSOLE_FONT_GREEN,
		CONSOLE_FONT_RED
	};

	namespace Platform
	{
		void SetLoggerConsoleFontColor(ConsoleFontColor fontColor);
		void ShowMessageBox(const UTF8String& msg);
		void LoadFileFromOpenWindow(const char* fileFilter, std::function<void(const std::string&)> callback);
		void SaveFileToOpenWindow(const char* fileFilter, std::function<void(const std::string&)> callback);
		void ShowBrowseForFolder(const char* title, std::function<void(const std::string&)> callback);
	}
}

