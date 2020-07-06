#pragma once

#include <string>

namespace Cjing3D
{
	class UTF8String;

	class UTF8Iterator final
	{
	public:
		UTF8Iterator() = delete;
		explicit UTF8Iterator(const UTF8String& str);
		UTF8Iterator(const UTF8Iterator& rhs);
		UTF8Iterator& operator=(const UTF8Iterator& rhs);

		const UTF8String::U8String operator*()const;

		UTF8Iterator operator+(size_t index);
		UTF8Iterator operator-(size_t index);
		UTF8Iterator& operator++();
		UTF8Iterator& operator--();
		size_t operator-(const UTF8Iterator& rhs)const;

		bool operator==(const UTF8Iterator& rhs)const;
		bool operator!=(const UTF8Iterator& rhs)const;
		bool operator<(const UTF8Iterator& rhs)const;
		bool operator>(const UTF8Iterator& rhs)const;
		bool operator<=(const UTF8Iterator& rhs)const;
		bool operator>=(const UTF8Iterator& rhs)const;

	private:
		UTF8String mString;
		size_t mIndex = 0;
	};
}