#pragma once

#include <string>
#include <vector>
#include <functional>

namespace Cjing3D {

	class Engine;

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

		// console
		static void SetLoggerConsoleFontColor(ConsoleFontColor fontColor);
		static void ShowMessageBox(const char* msg);
		static void LoadFileFromOpenWindow(const char* fileFilter, std::function<void(const std::string&)> callback);
		static void SaveFileToOpenWindow(const char* fileFilter, std::function<void(const std::string&)> callback);
		static void ShowBrowseForFolder(const char* title, std::function<void(const std::string&)> callback);
	};



}