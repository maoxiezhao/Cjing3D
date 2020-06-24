#include "utf8String.h"

namespace Cjing3D
{
	UTF8Iterator::UTF8Iterator(const UTF8String& str) :
		mString(str), mIndex(0)
	{
	}

	UTF8Iterator::UTF8Iterator(const UTF8Iterator& rhs) :
		mString(rhs.mString), mIndex(rhs.mIndex)
	{
	}

	UTF8Iterator& UTF8Iterator::operator=(const UTF8Iterator& rhs)
	{
		mString = rhs.mString;
		mIndex = rhs.mIndex;
		return *this;
	}

	const UTF8String::U8String UTF8Iterator::operator*() const
	{
		return mString.At(mIndex);
	}

	UTF8Iterator UTF8Iterator::operator+(size_t index)
	{
		UTF8Iterator it(*this);
		const size_t len = mString.Length();
		it.mIndex = mIndex + index < len ? mIndex + index : len;
		return it;
	}

	UTF8Iterator UTF8Iterator::operator-(size_t index)
	{
		UTF8Iterator it(*this);
		const size_t len = mString.Length();
		it.mIndex = mIndex - index >= 0 ? mIndex - index : 0;
		return it;
	}

	UTF8Iterator& UTF8Iterator::operator++()
	{
		if (mIndex < mString.Length())
			mIndex++;

		return *this;
	}

	UTF8Iterator& UTF8Iterator::operator--()
	{
		if (mIndex > 0)
			mIndex--;

		return *this;
	}

	size_t UTF8Iterator::operator-(const UTF8Iterator& rhs) const
	{
		return mIndex - rhs.mIndex;
	}

	bool UTF8Iterator::operator==(const UTF8Iterator& rhs) const
	{
		return (mString == rhs.mString) && (mIndex == rhs.mIndex);
	}

	bool UTF8Iterator::operator!=(const UTF8Iterator& rhs) const
	{
		return (mString != rhs.mString) || (mIndex != rhs.mIndex);
	}

	bool UTF8Iterator::operator<(const UTF8Iterator& rhs) const
	{
		return (mString == rhs.mString) && (mIndex < rhs.mIndex);
	}

	bool UTF8Iterator::operator>(const UTF8Iterator& rhs) const
	{
		return (mString == rhs.mString) && (mIndex > rhs.mIndex);
	}

	bool UTF8Iterator::operator<=(const UTF8Iterator& rhs) const
	{
		return (mString == rhs.mString) && (mIndex <= rhs.mIndex);
	}

	bool UTF8Iterator::operator>=(const UTF8Iterator& rhs) const
	{
		return (mString == rhs.mString) && (mIndex >= rhs.mIndex);
	}
}
