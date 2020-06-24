#pragma once

#include <string>
#include <windows.h>
#include <exception>
#include <sstream>

#include "common\definitions.h"

namespace Cjing3D {

	class Exception : public std::exception
	{
	public:
		Exception();
		Exception(const char* format, ...);
		Exception(const char* format, va_list args);
		virtual ~Exception();

		virtual const char* what() const noexcept override;

	private:
		char mMsg[2048];
	};

	namespace Debug
	{
		void SetDebugConsoleEnable(bool t);
		void InitializeDebugConsole();
		void UninitializeDebugConsolse();

		void SetDieOnError(bool t);
		void SetPopBoxOnDie(bool t);
		void SetAbortOnDie(bool t);

		void Warning(const std::string& warningMsg);
		void Error(const std::string& errorMsg);
		void Die(const std::string& dieMsg);

		void CheckAssertion(bool assertion);
		void CheckAssertion(bool assertion, const std::string& errorMsg);

		void ThrowIfFailed(bool result);
		void ThrowIfFailed(bool result, const char* format, ...);
		void ThrowIfFailed(HRESULT result);
		void ThrowIfFailed(HRESULT result, const char* format, ...);
		void ThrowInvalidArgument(const char* format, ...);
	}

#define ERR_FAIL_COND(mCond)																		\
	if (mCond) {																					\
		std::ostringstream oss;																		\
		oss << __FUNCTION__; oss << __FILE__; oss << __LINE__;										\
		oss << "Condition \"" _STR(mCond) "\" is true. returned: " _STR(mRet);					    \
		Debug::Error(oss.str());																	\
	} else                                                                                          \
		((void)0)

#define ERR_FAIL_COND_V(mCond, mRet)															    \
	if (mCond) {																					\
		std::ostringstream oss;																		\
		oss << __FUNCTION__; oss << __FILE__; oss << __LINE__;										\
		oss << "Condition \"" _STR(mCond) "\" is true. returned: " _STR(mRet);					    \
		Debug::Error(oss.str());																	\
		return mRet;                                                                                \
	} else                                                                                          \
		((void)0)

}