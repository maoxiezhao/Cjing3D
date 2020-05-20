#pragma once

#include "common\common.h"

namespace Cjing3D
{
	uint32_t atomicDecrement(volatile uint32_t* pw);
	uint32_t atomicIncrement(volatile uint32_t* pw);
	uint32_t atomicAdd(volatile uint32_t* pw, volatile uint32_t val);
	uint32_t atomicSub(volatile uint32_t* pw, volatile uint32_t val);
	uint32_t atomicExchangeIfGreater(volatile uint32_t* pw, volatile uint32_t val);

	uint64_t atomicDecrement(volatile uint64_t* pw);
	uint64_t atomicIncrement(volatile uint64_t* pw);
	uint64_t atomicAdd(volatile uint64_t* pw, volatile uint64_t val);
	uint64_t atomicSub(volatile uint64_t* pw, volatile uint64_t val);
	uint64_t atomicExchangeIfGreater(volatile uint64_t* pw, volatile uint64_t val);
}
