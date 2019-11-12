#pragma once

#include <string>
#include <map>

namespace Cjing3D {

	template<typename E>
	struct EnumInfo {
		using EnumType = std::map<E, std::string>;
	};

	/**
	*	static const std::string  enumName;
	*	static const EnumType<T>::EnumType enumInfos;
	*/
	template<typename E>
	struct EnumInfoTraits;

	template<typename E>
	std::string EnumToString(E value);

	template<typename E>
	std::string EnumToString(E value, const std::string& defaultValue);

	template<typename E>
	E StringToEnum(const std::string& name);

	template<typename E>
	E StringToEnum(const std::string& name, E defaultValue);


#define ENUM_TRAITS_REGISTER_ENUM_HEADER(ENUM_TYPE)   \
	template<>										\
	struct EnumInfoTraits<ENUM_TYPE>{		        \
		static const std::string enumName;       \
		static const EnumInfo<ENUM_TYPE>::EnumType enumInfos;};

#define ENUM_TRAITS_REGISTER_ENUM_BEGIN(ENUM_TYPE)   \
    const std::string EnumInfoTraits<ENUM_TYPE>::enumName = #ENUM_TYPE;\
	const EnumInfo<ENUM_TYPE>::EnumType EnumInfoTraits<ENUM_TYPE>::enumInfos = {

#define ENUM_TRAITS_REGISTER_ENUM_DEFINE(ENUM_VALUE) {ENUM_VALUE, #ENUM_VALUE},

#define ENUM_TRAITS_REGISTER_ENUM_END(ENUM_TYPE) };

#include "enumInfo.inl"

}