#include "stringID.h"

namespace Cjing3D {

	StringID::StringID() :
		mValue(0),
		mStr()
	{
	}

	StringID::StringID(const char * str) :
		mValue(CalculateHash(str)),
		mStr(str)
	{
	}

	StringID::StringID(const std::string & str) :
		mValue(CalculateHash(str.c_str())),
		mStr(str)
	{
	}

	StringID::StringID(const StringID & rhs) :
		mValue(rhs.mValue),
		mStr(rhs.mStr)
	{
	}

	StringID::StringID(StringID && rhs) = default;

	StringID::~StringID() = default;

	StringID & StringID::operator=(const StringID & rhs)
	{
		mValue = rhs.mValue;
		return *this;
	}

	StringID & StringID::operator=(StringID && rhs) = default;

	U32 StringID::CalculateHash(const char * str)
	{
		if (str == nullptr)
			return 0;

		U32 hashValue = 0;
		while (*str != 0) {
			hashValue = SDBMHash(hashValue, (unsigned char)*str++);
		}

		return hashValue;
	}

}
