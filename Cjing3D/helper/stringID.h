#pragma once

#include "common\definitions.h"

#include <algorithm>
#include <string>
#include <map>

namespace Cjing3D {

	class Archive;

	/**
	*	\brief 32bit hash String
	*/
	LUA_BINDER_REGISTER_CLASS
	class StringID final
	{
	public:
		StringID();
		StringID(const char* str);

		LUA_BINDER_REGISTER_CLASS_CONSTRUCTOR
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

		virtual void Serialize(Archive& archive, uint32_t seed = 0);
		virtual void Unserialize(Archive& archive)const;

	private:
		unsigned int mValue;
	};

#define STRING_ID(key) StringID(#key)
}