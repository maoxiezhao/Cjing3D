#pragma once

#include "common\common.h"

#include <map>

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

		inline unsigned int HashValue()const { return mValue; }
		std::string GetString()const;
		void SetString(const std::string& str);

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

		bool operator == (const StringID& rhs)const { return mValue == rhs.mValue; }
		bool operator != (const StringID& rhs)const { return mValue != rhs.mValue; }
		bool operator < (const StringID& rhs)const { return mValue < rhs.mValue; }
		bool operator > (const StringID& rhs)const { return mValue > rhs.mValue; }

		static unsigned int	CalculateHash(const char* str);
		static StringID EMPTY;
		static std::map<unsigned int, std::string> mHashStringMap;

	private:
		unsigned int mValue;
	};

#define STRING_ID(key) StringID(#key)
}