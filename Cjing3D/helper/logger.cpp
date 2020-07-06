#include "logger.h"
#include "platform\gameWindow.h"

#include <fstream>
#include <chrono>
#include <iostream>
#include <stdarg.h>

namespace Cjing3D {
	namespace Logger {
		using std::string;

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
		}

		void Logger::Print(const string & msg, std::ostream & out)
		{
			auto timeStr = GetCurSystemTimeStr();
			out << timeStr << "    " << msg << std::endl;
		}

		void PrintConsoleHeader()
		{
			std::cout << "Cjing3D Version 0.0.1\n" << std::endl;
			std::cout << "Copyright (c) 2019-2020 by ZZZY." << std::endl;
			std::cout << std::endl;
		}

		void Debug(const string & msg)
		{
			Print("Debug: " + msg);
		}

		void Logger::Info(const string & msg)
		{
			GameWindow::GameWindow::SetLoggerConsoleFontColor(CONSOLE_FONT_GREEN);
			Print("[Info]  " + msg);
			GameWindow::GameWindow::SetLoggerConsoleFontColor(CONSOLE_FONT_WHITE);
		}

		void InfoEx(const char* format, ...)
		{
			char msg[256] = {};
			va_list args;
			va_start(args, format);
			vsnprintf_s(msg, std::size(msg), format, args);
			va_end(args);

			GameWindow::SetLoggerConsoleFontColor(CONSOLE_FONT_GREEN);
			Print("[Info]  " + std::string(msg));
			GameWindow::SetLoggerConsoleFontColor(CONSOLE_FONT_WHITE);
		}

		void Logger::Warning(const string & msg)
		{
			GameWindow::SetLoggerConsoleFontColor(CONSOLE_FONT_YELLOW);
			string warningMsg = "[Warning]  " + msg;
			Print(warningMsg);
			Print(warningMsg, GetErrorFile());
			GameWindow::SetLoggerConsoleFontColor(CONSOLE_FONT_WHITE);
		}

		void Logger::Error(const string & msg)
		{
			GameWindow::SetLoggerConsoleFontColor(CONSOLE_FONT_RED);
			string warningMsg = "[Error]  " + msg;
			Print(warningMsg);
			Print(warningMsg, GetErrorFile());
			GameWindow::SetLoggerConsoleFontColor(CONSOLE_FONT_WHITE);
		}

		void Fatal(const string& msg)
		{
			GameWindow::SetLoggerConsoleFontColor(CONSOLE_FONT_WHITE);
			string warningMsg = "[Fatal]  " + msg;
			Print(warningMsg);
			Print(warningMsg, GetErrorFile());
		}
	}
}