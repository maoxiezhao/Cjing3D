#pragma once

#include "common\common.h"

#include <memory>
#include <vector>

namespace Cjing3D
{

class SubSystem : public std::enable_shared_from_this<SubSystem>
{
public:
	SubSystem() = default;
	virtual ~SubSystem() = default;

	virtual void Uninitialize() {}
};

}