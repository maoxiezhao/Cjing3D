#pragma once

#include <stdint.h>

namespace Cjing3D
{
namespace Random
{
	uint32_t GetRandomInt(uint32_t x);
	uint32_t GetRandomInt(uint32_t x, uint32_t y);
	float GetRandomFloat(float x = 1.0f);
	float GetRandomFloat(float x, float y);
	double GetRandomDouble(double x, double y);
	double GetRandomDouble(double x = 1.0);
}
}