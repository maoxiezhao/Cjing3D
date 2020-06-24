#pragma once

#include <string>
#include <vector>

// simple utf8 string based on utf8 library
// utf8.h: https://github.com/sheredom/utf8.h

namespace Cjing3D
{
	class UTF8Iterator;

	class UTF8String final
	{
	public:
		using U8Char = unsigned char;
		using U8String = std::string;
		static constexpr size_t npos = std::string::npos;

		UTF8String() = default;
		UTF8String(const char* str);
		UTF8String(const U8String& str);
		UTF8String(const UTF8String& rhs) noexcept;
		UTF8String(UTF8String&& rhs) noexcept;
		UTF8String& operator=(const UTF8String& rhs) noexcept;
		UTF8String& operator=(UTF8String&& rhs) noexcept;

		UTF8String& operator=(const char* str);
		UTF8String& operator=(const U8String& str);
		const UTF8String& operator+=(const char* str);
		const UTF8String& operator+=(const U8String& str);
		const UTF8String& operator+=(const UTF8String& str);
		U8String operator[](const size_t index)const;

		bool IsValid()const;
		size_t Length() const; 
		size_t Size() const;
		bool Empty()const;
		void Clear();
		U8String At(const size_t index)const;
		UTF8String AtUTF8(const size_t index)const;
		void Pop();
		UTF8String& Erase(size_t pos, size_t len = npos);
		UTF8String Substr(size_t pos, size_t len = npos);
		size_t Find(const UTF8String& str, size_t pos = 0);
		U8String& String();
		const U8String& String()const;
		const char* C_Str();

		UTF8Iterator iterator()const;
		UTF8Iterator begin()const;
		UTF8Iterator end()const;

		std::vector<int32_t> GetCodePoints()const;

	private:
		UTF8String& Assign(const char* str);
		UTF8String& Assign(const U8String& str);

	private:
		U8String mString;
		size_t mLength = 0;
	};

	bool operator ==(const UTF8String& str1, const UTF8String& str2) noexcept;
	bool operator !=(const UTF8String& str1, const UTF8String& str2) noexcept;
	bool operator <(const UTF8String& str1, const UTF8String& str2) noexcept;
	bool operator <=(const UTF8String& str1, const UTF8String& str2) noexcept;
	bool operator >(const UTF8String& str1, const UTF8String& str2) noexcept;
	bool operator >=(const UTF8String& str1, const UTF8String& str2) noexcept;

	UTF8String operator +(const UTF8String& str1, const UTF8String& str2);
	UTF8String operator +(const UTF8String& str1, const std::string& str2);
	UTF8String operator +(const std::string& str1, const UTF8String& str2);
	UTF8String operator +(const UTF8String& str1, const char* str2);
	UTF8String operator +(const char* str1, const UTF8String& str2);
}

#include "utf8StringIt.h"