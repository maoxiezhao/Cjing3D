#include "debug.h"
#include "logger.h"
#include "..\platform\gameWindow.h"

#include <stdarg.h>
#include <stdexcept>
#include <windows.h>

namespace Cjing3D
{
	Exception::Exception() :
		std::exception(),
		mMsg{}
	{
	}

	Exception::Exception(const char* format, ...) :
		Exception()
	{
		va_list args;
		va_start(args, format);
		vsnprintf_s(mMsg, std::size(mMsg), format, args);
		va_end(args);

		Debug::Error(mMsg);
	}

	Exception::Exception(const char* format, va_list args) :
		Exception()
	{
		vsnprintf_s(mMsg, std::size(mMsg), format, args);

		Debug::Error(mMsg);
	}

	Exception::~Exception() = default;

	const char* Exception::what() const noexcept
	{
		return mMsg;
	}

	namespace Debug {
		namespace {
			bool ShowDebugConsole = false;
			bool ShowMsgBox = false;
			bool AbortOnDie = false;
			bool DieOnError = false;
			bool debugWarningPause = true;
		}

		void InitializeDebugConsole()
		{
			if (ShowDebugConsole)
			{
				{
					bool result = AllocConsole();
					Debug::ThrowIfFailed(result, "Alloc console failed.");
				}
				{
					FILE* stream;
					int result = freopen_s(&stream, "CONOUT$", "w+t", stdout);
					Debug::ThrowIfFailed(result == 0, "stdout redirection failed.");

					std::cout.clear();
				}
				{
					FILE* stream;
					int result = freopen_s(&stream, "CONIN$", "r+t", stdin);
					Debug::ThrowIfFailed(result == 0, "stdin redirection failed.");
				}
			}
		}

		void UninitializeDebugConsolse()
		{
			FreeConsole();
		}

		void SetDebugConsoleEnable(bool t)
		{
			ShowDebugConsole = t;
		}

		void SetDieOnError(bool t)
		{
			DieOnError = t;
		}

		void SetPopBoxOnDie(bool t)
		{
			ShowMsgBox = t;
		}

		void SetAbortOnDie(bool t)
		{
			AbortOnDie = t;
		}

		void Warning(const std::string& warningMsg)
		{
			Logger::Warning(warningMsg);
		}

		void Error(const std::string& errorMsg)
		{
			Logger::Error(errorMsg);
			if (DieOnError)
				abort();
		}

		void CheckAssertion(bool asertion)
		{
			if (!asertion)
				std::abort();
		}

		void CheckAssertion(bool assertion, const std::string& errorMsg)
		{
			if (!assertion) {
				Die(errorMsg);
			}
		}

		void ThrowIfFailed(bool result)
		{
			if (false == result) {
				throw Exception();
			}
		}

		void ThrowIfFailed(bool result, const char* format, ...)
		{
			if (false == result) {
				va_list args;
				va_start(args, format);
				Exception exception(format, args);
				va_end(args);

				throw exception;
			}
		}

		void ThrowIfFailed(HRESULT result)
		{
			if (FAILED(result)) {
				throw Exception();
			}
		}

		void ThrowIfFailed(HRESULT result, const char* format, ...)
		{
			if (FAILED(result)) {
				va_list args;
				va_start(args, format);
				Exception exception(format, args);
				va_end(args);

				throw exception;
			}
		}

		void ThrowInvalidArgument(const char* format, ...)
		{
			char msg[128];
			va_list args;
			va_start(args, format);
			vsnprintf_s(msg, std::size(msg), format, args);
			va_end(args);
			throw std::invalid_argument(msg);
		}

		void Die(const std::string& dieMsg)
		{
			Logger::Fatal(dieMsg);
			if (ShowMsgBox) {
				GameWindow::ShowMessageBox(dieMsg.c_str());
			}

			if (AbortOnDie) {
				std::abort();
			}

			Exception exception(dieMsg.c_str());
			throw exception;
		}

	}
}