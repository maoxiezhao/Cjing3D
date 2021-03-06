#include "stringID.h"
#include "utils\math.h"
#include "utils\string\utf8String.h"
#include "helper\archive.h"

namespace Cjing3D {

	StringID StringID::EMPTY = StringID();
	std::map<unsigned int, std::string> StringID::mHashStringMap;

	StringID::StringID() :
		mValue(0)
	{
	}

	StringID::StringID(const char* str) :
		mValue(CalculateHash(str))
	{
		mHashStringMap[mValue] = str;
	}

	StringID::StringID(const std::string & str) :
		mValue(CalculateHash(str.c_str()))
	{
		mHashStringMap[mValue] = str;
	}

	StringID::StringID(const StringID & rhs) :
		mValue(rhs.mValue)
	{
	}

	StringID::StringID(StringID && rhs) = default;

	StringID::StringID(const UTF8String & str) :
		StringID(str.String())
	{
	}

	StringID::~StringID() = default;

	StringID & StringID::operator=(const StringID & rhs)
	{
		mValue = rhs.mValue;
		return *this;
	}

	StringID & StringID::operator=(StringID && rhs) = default;

	std::string StringID::GetString() const
	{
		auto it = mHashStringMap.find(mValue);
		return it != mHashStringMap.end() ? it->second : "";
	}

	void StringID::SetString(const std::string & str)
	{
		mValue = CalculateHash(str.data());
		mHashStringMap[mValue] = str;
	}

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

	void StringID::Serialize(Archive& archive, uint32_t seed)
	{
		// 直接序列化hash value，但会导致GetString失效
		std::string string;
		archive >> string;

		SetString(string);
	}

	void StringID::Unserialize(Archive& archive) const
	{
		auto string = GetString();
		archive << string;
	}

}
