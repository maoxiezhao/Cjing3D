#pragma once

#include "common\common.h"

namespace Cjing3D {

	/**
	*	\brief 32bit hash String
	*/
	class StringID final
	{
	public:
		StringID();
		StringID(const char* str);
		StringID(const std::string& str);
		StringID(const StringID& rhs);
		StringID(StringID&& rhs);
		~StringID();

		StringID& operator= (const StringID& rhs);
		StringID& operator= (StringID&& rhs);

		unsigned int HashValue()const { return mValue; }
		std::string GetString()const { return mStr; }

		operator bool()const { return mValue != 0; }

		StringID operator + (const StringID& rhs) {
			StringID ret;
			ret.mValue = mValue + rhs.mValue;
			return ret;
		}

		StringID& operator += (const StringID& rhs) {
			mValue += rhs.mValue;
			return *this;
		}

		bool operator == (const StringID& rhs) { return mValue == rhs.mValue; }
		bool operator != (const StringID& rhs) { return mValue != rhs.mValue; }
		bool operator < (const StringID& rhs) { return mValue < rhs.mValue; }
		bool operator > (const StringID& rhs) { return mValue > rhs.mValue; }

		static unsigned int	CalculateHash(const char* str);

	private:
		int mValue;
		std::string mStr;
	};

#define STRING_ID(key) StringID(#key)
}