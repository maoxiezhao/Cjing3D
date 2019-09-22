#pragma once

template<typename E>
std::string EnumToString(E value)
{
	const auto& it = EnumInfoTraits<E>::enumInfos.find(value);
	if (it == EnumInfoTraits<E>::enumInfos.end()) {
		Debug::Die(std::string("Invalid") + EnumInfoTraits<E>::enumName + " number:" +
			std::to_string(static_cast<int>(value)));
	}
	return it->second;
}

template<typename E>
std::string EnumToString(E value, const std::string& defaultValue)
{
	const auto& it = EnumInfoTraits<E>::enumInfos.find(value);
	if (it == EnumInfoTraits<E>::enumInfos.end()) {
		return defaultValue;
	}
	return it->second;
}


template<typename E>
E StringToEnum(const std::string & name)
{
	for (auto&it : EnumInfoTraits<E>::enumInfos){
		if (it.second == name) {
			return it.first;
		}
	}

	Debug::Die(std::string("Invalid") + EnumInfoTraits<E>::enumName + " name:" + name);
}

template<typename E>
E StringToEnum(const std::string& name, E defaultValue)
{
	for (auto&it : EnumInfoTraits<E>::enumInfos) {
		if (it.second == name) {
			return it.first;
		}
	}
	return defaultValue;
}