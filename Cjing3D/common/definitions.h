#pragma once

#define NOMINMAX
#undef min
#undef max

#ifdef _MSC_VER
#pragma warning( disable :4005 4018 4522 4715 4800 4996 4267 26812)
#endif

#ifndef _ALWAYS_INLINE_

#if defined(__GNUC__) && (__GNUC__ >= 4)
#define _ALWAYS_INLINE_ __attribute__((always_inline)) inline
#elif defined(__llvm__)
#define _ALWAYS_INLINE_ __attribute__((always_inline)) inline
#elif defined(_MSC_VER)
#define _ALWAYS_INLINE_ __forceinline
#else
#define _ALWAYS_INLINE_ inline
#endif

#endif

//should always inline, except in some cases because it makes debugging harder
#ifndef _FORCE_INLINE_

#ifdef DISABLE_FORCED_INLINE
#define _FORCE_INLINE_ inline
#else
#define _FORCE_INLINE_ _ALWAYS_INLINE_
#endif

#endif

#define SAFE_DELETE(p) if(p!= nullptr) {delete (p); p=nullptr;}
#define SAFE_DELETE_ARRAY(p) if(p!= nullptr) {delete[](p); p=nullptr;}

#define CJING_DEBUG
#define CJING_IMGUI_ENABLE

// lua binding
#define LUA_BINDER_REGISTER_CLASS 
#define LUA_BINDER_REGISTER_CLASS_CONSTRUCTOR
#define LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
#define LUA_BINDER_REGISTER_CLASS_STATIC_FUNCTION
#define LUA_BINDER_REGISTER_CLASS_SET_NAME(name)

// common definitions
#ifndef _STR
#define _STR(m_x) #m_x
#define _MKSTR(m_x) _STR(m_x)
#endif

#define PLATFORM_ALIGNMENT 16
#define ALIGN_TO(n, a)((n + a)&~a)

namespace Cjing3D
{
	using U8 = unsigned __int8;
	using U16 = unsigned __int16;
	using U32 = unsigned __int32;
	using U64 = unsigned __int64;

	using I8 = __int8;
	using I16 = __int16;
	using I32 = __int32;
	using I64 = __int64;

	using F32 = float;
	using F64 = double;
}

#include <wrl.h>
template<typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;