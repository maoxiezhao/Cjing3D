#include "utils\string\utf8String.h"
#include "utils\string\utf8.h"
#include "helper\debug.h"

#include <algorithm>

namespace Cjing3D
{
#ifdef __cplusplus
#define utf8_null NULL
#else
#define utf8_null 0
#endif

	namespace 
	{
		bool CheckUTF8StringValid(const std::string& str)
		{
			return utf8valid(str.data()) == utf8_null;
		}

		size_t GetUTF8PosAt(const std::string& str, size_t index)
		{
			size_t pos = 0;	
			const char* v = str.data();
			for (size_t i = 0; i < index && *v; i++)
			{
				size_t size = utf8codepointcalcsize(v);
				pos += size;
				v += size;
			}
			return pos;
		}

		std::string SubstrUTF8String(const std::string& str, size_t startPos, size_t endPos)
		{
			// TODO
			std::string result;
			const char* v = str.data();
			for (size_t i = 0; *v && i < endPos; i++)
			{
				size_t size = utf8codepointcalcsize(v);
				if (i >= startPos && size > 0) {
					for (size_t index = 0; index < size; index++)
						result += *(v + index);
				}
				v += size;
			}
			return result;
		}

		size_t FindUTF8String(const std::string& hayStack, const std::string& needle, size_t startPos = 0)
		{
			const char* v = hayStack.data();
			if (startPos > 0) {
				v += GetUTF8PosAt(hayStack, startPos);
			}

			void* ret = utf8str(v, needle.data());
			if (ret == utf8_null) {
				return UTF8String::npos;
			}
			size_t pos = 0;

			for (; *v && v != ret; v += utf8codepointcalcsize(v)) {
				pos++;
			}
			return pos;
		}
	}

	UTF8String::UTF8String(const char* str) :
		UTF8String(std::string(str))
	{
	}

	UTF8String::UTF8String(const U8String& str) :
		mString(str)
	{
		if (!IsValid()) {
			Debug::ThrowInvalidArgument("Invalid UTF-8 stirng.");
		}
		mLength = utf8len(str.data());
	}

	UTF8String::UTF8String(const UTF8String& rhs) noexcept :
		mString(rhs.mString), mLength(rhs.mLength)
	{
	}

	UTF8String::UTF8String(UTF8String&& rhs) noexcept :
		mString(std::move(rhs.mString)), mLength(rhs.mLength)
	{
		rhs.mLength = 0;
	}

	UTF8String& UTF8String::operator=(const UTF8String& rhs) noexcept
	{
		mString = rhs.mString;
		mLength = rhs.mLength;
		return *this;
	}

	UTF8String& UTF8String::operator=(UTF8String&& rhs) noexcept
	{
		mString = std::move(rhs.mString);
		mLength = rhs.mLength;
		return *this;
	}

	UTF8String& UTF8String::operator=(const char* str)
	{
		return Assign(str);
	}

	UTF8String& UTF8String::operator=(const U8String& str)
	{
		return Assign(str);
	}

	const UTF8String& UTF8String::operator+=(const char* str)
	{
		if (!CheckUTF8StringValid(str)) {
			Debug::ThrowInvalidArgument("Invalid UTF-8 stirng.");
		}
		mString += str;
		mLength = utf8len(mString.data());
		return *this;
	}

	const UTF8String& UTF8String::operator+=(const U8String& str)
	{
		if (!CheckUTF8StringValid(str)) {
			Debug::ThrowInvalidArgument("Invalid UTF-8 stirng.");
		}
		mString += str;
		mLength = utf8len(mString.data());
		return *this;
	}

	const UTF8String& UTF8String::operator+=(const UTF8String& str)
	{
		mString += str.mString;
		mLength = utf8len(mString.data());
		return *this;
	}

	UTF8String::U8String UTF8String::operator[](const size_t index) const
	{
		return At(index);
	}

	size_t UTF8String::Length() const
	{
		return mLength;
	}

	size_t UTF8String::Size() const
	{
		return utf8size(mString.data());
	}

	bool UTF8String::Empty() const
	{
		return mLength <= 0;
	}

	void UTF8String::Clear()
	{
		mString.clear();
		mLength = 0;
	}

	UTF8String::U8String UTF8String::At(const size_t index) const
	{
		size_t pos = GetUTF8PosAt(mString, index);
		return mString.substr(pos, utf8codepointcalcsize(mString.data()));
	}

	UTF8String UTF8String::AtUTF8(const size_t index) const
	{
		return UTF8String(At(index));
	}

	UTF8String::U8String& UTF8String::String()
	{
		return mString;
	}

	const UTF8String::U8String& UTF8String::String() const
	{
		return mString;
	}

	const char* UTF8String::C_Str()
	{
		return mString.c_str();
	}

	UTF8Iterator UTF8String::iterator() const
	{
		return UTF8Iterator(*this);
	}

	UTF8Iterator UTF8String::begin() const
	{
		return iterator();
	}

	UTF8Iterator UTF8String::end() const
	{
		return iterator() + mLength;
	}

	std::vector<int32_t> UTF8String::GetCodePoints() const
	{
		std::vector<int32_t> ret;
		utf8_int32_t codepoint;
		const char* data = mString.data();
		for (void* v = utf8codepoint(data, &codepoint); codepoint; v = utf8codepoint(v, &codepoint)) {
			ret.push_back(codepoint);
		}
		return ret;
	}

	void UTF8String::Pop()
	{
		size_t pos = GetUTF8PosAt(mString, mLength - 1);
		mString.erase(pos);
		mLength--;
	}

	UTF8String UTF8String::Substr(size_t pos, size_t len)
	{
		if (pos >= mLength) {
			return UTF8String();
		}

		size_t substrCount = (len == npos || (pos + len > mLength)) ? mLength - pos : len;
		return UTF8String(SubstrUTF8String(mString, pos, pos + substrCount));
	}

	size_t UTF8String::Find(const UTF8String& str, size_t pos)
	{
		if (str.Length() <= 0) {
			return npos;
		}

		return FindUTF8String(mString, str.mString, pos);
	}

	UTF8String& UTF8String::Erase(size_t pos, size_t len)
	{
		if (pos >= mLength) {
			return *this;
		}

		const size_t eraseCount = std::max((size_t)0, std::min(len, mLength - pos));
		if (eraseCount <= 0) {
			return *this;
		}

		mString = SubstrUTF8String(mString, pos, pos + eraseCount);
		mLength = eraseCount;
	}

	UTF8String& UTF8String::Assign(const char* str)
	{
		if (!CheckUTF8StringValid(str)) {
			Debug::ThrowInvalidArgument("Invalid UTF-8 stirng.");
		}
		mString = std::string(str);
		mLength = utf8len(str);

		return *this;
	}

	UTF8String& UTF8String::Assign(const U8String& str)
	{
		if (!CheckUTF8StringValid(str)) {
			Debug::ThrowInvalidArgument("Invalid UTF-8 stirng.");
		}
		mString = std::string(str);
		mLength = utf8len(str.data());

		return *this;
	}

	bool UTF8String::IsValid() const
	{
		return utf8valid(mString.data()) == utf8_null;
	}

#undef utf8_null

	bool operator==(const UTF8String& str1, const UTF8String& str2) noexcept
	{
		return str1.String() == str2.String();
	}

	bool operator!=(const UTF8String& str1, const UTF8String& str2) noexcept
	{
		return str1.String() != str2.String();
	}

	bool operator<(const UTF8String& str1, const UTF8String& str2) noexcept
	{
		return str1.String() < str2.String();
	}

	bool operator<=(const UTF8String& str1, const UTF8String& str2) noexcept
	{
		return str1.String() <= str2.String();
	}

	bool operator>(const UTF8String& str1, const UTF8String& str2) noexcept
	{
		return str1.String() > str2.String();
	}

	bool operator>=(const UTF8String& str1, const UTF8String& str2) noexcept
	{
		return str1.String() >= str2.String();
	}

	UTF8String operator+(const UTF8String& str1, const UTF8String& str2)
	{
		return str1 + str2.String();
	}

	UTF8String operator+(const UTF8String& str1, const std::string& str2)
	{
		return UTF8String(str1.String() + str2);
	}

	UTF8String operator+(const std::string& str1, const UTF8String& str2)
	{
		return UTF8String(str1 + str2.String());
	}

	UTF8String operator+(const UTF8String& str1, const char* str2)
	{
		return str1 + std::string(str2);
	}

	UTF8String operator+(const char* str1, const UTF8String& str2)
	{
		return std::string(str1) + str2;
	}



}