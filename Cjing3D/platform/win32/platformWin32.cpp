#ifdef _WIN32
#include "platform\platform.h"

#include <windows.h>
#include <string>
#include <ShlObj.h>

namespace Cjing3D::Platform
{
	std::string WStringToString(const std::wstring& wstr)
	{
		if (wstr.empty()) return std::string();
		int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
		std::string strTo(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
		return strTo;
	}

	// Convert an UTF8 string to a wide Unicode String
	std::wstring StringToWString(const std::string& str)
	{
		if (str.empty()) return std::wstring();
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
		std::wstring wstrTo(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
		return wstrTo;
	}

	void SetLoggerConsoleFontColor(ConsoleFontColor fontColor)
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

	void LoadFileFromOpenWindow(const char* fileFilter, std::function<void(const std::string&)> callback)
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

	void SaveFileToOpenWindow(const char* fileFilter, std::function<void(const std::string&)> callback)
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

	void ShowBrowseForFolder(const char* title, std::function<void(const std::string&)> callback)
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

	void ShowMessageBox(const UTF8String& msg)
	{
		std::wstring msgWStr = StringToWString(msg.C_Str());
		MessageBoxW(NULL, LPCWSTR(msgWStr.c_str()), NULL, MB_OK);
	}
}

#endif