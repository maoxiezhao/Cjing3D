#pragma once

#include "scripts\luaTools.h"
#include "scripts\luaRef.h"

#include <tuple>

namespace Cjing3D {

template<typename T>
struct LuaArgHolder
{
    T& Get() { return mValue; }
    const T& Get()const { return mValue; }
    void Set(const T& value) { mValue = value;}
    T mValue;
};
template<typename T>
struct LuaArgHolder<T&>
{
    T& Get() { return *mValue; }
    void Set(T& value) { mValue = &value;}
    T* mValue;
};

template<typename T>
struct LuaArgTypeTraits 
{
	// 不可直接使用LuaArgHolder<T>，对于func(const std::string& str)
	// 会偏特化为LuaArgHolder<T&>，而LuaType<std::string>::Get则只会返回std::string而出错
	// 故这里使用推导以LuaType<std::string>::Get的返回类型为准

	using ArgType = typename std::result_of<decltype(&LuaType<T>::Get)(lua_State*, int)>::type;	 
	using ArgHolderType = LuaArgHolder<ArgType>;
};

// 从lua stack 中取出数据，并将数据赋给LuaArgHover
template<typename ArgTypeTraits>
struct LuaArgGetter
{
	using ArgHolderType = typename ArgTypeTraits::ArgHolderType;
	using ArgType = typename ArgTypeTraits::ArgType;

    static int Get(lua_State*l, int index, ArgHolderType& holder)
    {
        holder.Set(LuaTools::Get<ArgType>(l, index));
        return 1;
    }
};

template<typename T>
struct LuaArgSetter;

template<typename T>
struct LuaArgs
{
	using LuaArgTraits = LuaArgTypeTraits<T>;
	using LuaArgHolderType = typename LuaArgTraits::ArgHolderType;

    static int Get(lua_State*l, int index, LuaArgHolderType& holder)
    {
        return LuaArgGetter<LuaArgTraits>::Get(l, index, holder);
    }
};

// 依次遍历获取各个参数
template<typename... Args>
struct LuaInputArgs;

template<>
struct LuaInputArgs<> 
{
    template<typename... T>
    static void Get(lua_State*l, int index, std::tuple<T...>& valueTuple) {}   
};

template<typename P, typename... Args>
struct LuaInputArgs<P, Args... >
{
    template<typename... T>
    static void Get(lua_State*l, int index, std::tuple<T...>& valueTuple)
    {
        // handle current arg
        auto& value = std::get<sizeof...(T) - sizeof...(Args) - 1>(valueTuple);
        index += LuaArgs<P>::Get(l, index, value);
        return LuaInputArgs<Args...>::Get(l, index, valueTuple);
    }
};

template<typename... Args>
using LuaArgValueTuple = std::tuple<typename LuaArgs<Args>::LuaArgHolderType...>;

}