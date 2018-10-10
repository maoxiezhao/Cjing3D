#include "logger.h"

#include <fstream>
#include <chrono>
#include <windows.h>
#include <iostream>

using std::string;

namespace Cjing3D {
	namespace Logger {
		namespace {
			const string errorLogFileName = "error.txt";
			std::ofstream errorFile;

			std::ofstream& GetErrorFile()
			{
				if (!errorFile.is_open())
					errorFile.open(errorLogFileName);
				return errorFile;
			}

			std::string GetCurSystemTimeStr()
			{
				auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
				tm ptm;
				localtime_s(&ptm, &tt);
				char date[60] = { 0 };
				sprintf_s(date, "%02d:%02d:%02d ",
					(int)ptm.tm_hour, (int)ptm.tm_min, (int)ptm.tm_sec);
				return std::string(date);
			}

			const int CONSOLE_FONT_WHITE = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
			const int CONSOLE_FONT_BLUE = FOREGROUND_BLUE;
			const int CONSOLE_FONT_YELLOW = FOREGROUND_RED | FOREGROUND_GREEN;
			const int CONSOLE_FONT_GREEN = FOREGROUND_GREEN;
			const int CONSOLE_FONT_RED = FOREGROUND_RED;
			void SetLoggerConsoleFontColor(int fontColor)
			{
				HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
				SetConsoleTextAttribute(handle, FOREGROUND_INTENSITY | static_cast<WORD>(fontColor));
			}
		}

		void Logger::Print(const string & msg, std::ostream & out)
		{
			auto timeStr = GetCurSystemTimeStr();
			out << timeStr << "    " << msg << std::endl;
		}

		void PrintConsoleHeader()
		{
			std::cout << "Cjing-d3d Version 0.0.1" << std::endl;
			std::cout << "Copyright (c) 2017-2018 ZZZY." << std::endl;
			std::cout << std::endl;
		}

		void Debug(const string & msg)
		{
			Print("Debug: " + msg);
		}

		void Logger::Info(const string & msg)
		{
			SetLoggerConsoleFontColor(CONSOLE_FONT_GREEN);
			Print("[Info]  " + msg);
			SetLoggerConsoleFontColor(CONSOLE_FONT_WHITE);
		}

		void Logger::Warning(const string & msg)
		{
			SetLoggerConsoleFontColor(CONSOLE_FONT_YELLOW);
			string warningMsg = "[Warning]  " + msg;
			Print(warningMsg);
			Print(warningMsg, GetErrorFile());
			SetLoggerConsoleFontColor(CONSOLE_FONT_WHITE);
		}

		void Logger::Error(const string & msg)
		{
			SetLoggerConsoleFontColor(CONSOLE_FONT_RED);
			string warningMsg = "[Error]  " + msg;
			Print(warningMsg);
			Print(warningMsg, GetErrorFile());
			SetLoggerConsoleFontColor(CONSOLE_FONT_WHITE);
		}

		void Fatal(const string& msg)
		{
			SetLoggerConsoleFontColor(CONSOLE_FONT_WHITE);
			string warningMsg = "[Fatal]  " + msg;
			Print(warningMsg);
			Print(warningMsg, GetErrorFile());
		}
	}
}