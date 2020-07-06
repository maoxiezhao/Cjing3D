#ifdef _WIN32

#include "core\safeRefcount.h"
#include "platform\win32\gameWindowWin32.h"

uint32_t Cjing3D::atomicDecrement(volatile uint32_t* pw)
{
	return InterlockedDecrement((LONG volatile*)pw);
}

uint32_t Cjing3D::atomicIncrement(volatile uint32_t* pw)
{
	return InterlockedIncrement((LONG volatile*)pw);
}

uint32_t Cjing3D::atomicAdd(volatile uint32_t* pw, volatile uint32_t val)
{
	return InterlockedAdd((LONG volatile*)pw, val);
}

uint32_t Cjing3D::atomicSub(volatile uint32_t* pw, volatile uint32_t val)
{
	return InterlockedExchangeAdd((LONG volatile*)pw, -(int32_t)val) - val;
}

uint32_t Cjing3D::atomicExchangeIfGreater(volatile uint32_t* pw, volatile uint32_t val)
{
	while (true)
	{
		uint32_t tmp = static_cast<uint32_t const volatile&>(*(pw));
		if (tmp >= val) {
			return tmp;
		}

		if (InterlockedCompareExchange((LONG volatile*)(pw), val, tmp) == tmp) {
			return val;
		}
	}
}

uint64_t Cjing3D::atomicDecrement(volatile uint64_t* pw)
{
	return InterlockedDecrement64((LONGLONG volatile*)pw);
}

uint64_t Cjing3D::atomicIncrement(volatile uint64_t* pw)
{
	return InterlockedIncrement64((LONGLONG volatile*)pw);
}

uint64_t Cjing3D::atomicAdd(volatile uint64_t* pw, volatile uint64_t val)
{
	return InterlockedAdd64((LONGLONG volatile*)pw, val);
}

uint64_t Cjing3D::atomicSub(volatile uint64_t* pw, volatile uint64_t val)
{
	return InterlockedExchangeAdd64((LONGLONG volatile*)pw, -(int64_t)val) - val;
}

uint64_t Cjing3D::atomicExchangeIfGreater(volatile uint64_t* pw, volatile uint64_t val)
{
	while (true)
	{
		uint64_t tmp = static_cast<uint64_t const volatile&>(*(pw));
		if (tmp >= val) {
			return tmp;
		}

		if (InterlockedCompareExchange64((LONGLONG volatile*)(pw), val, tmp) == tmp) {
			return val;
		}
	}
}

#endif