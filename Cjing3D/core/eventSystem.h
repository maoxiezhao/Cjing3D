#pragma once

#include "common\common.h"
#include "helper\variant.h"
#include "core\eventDefine.h"
#include "utils\thread\spinLock.h"
#include "utils\signal\signal.h"

#include <functional>
#include <unordered_map>

namespace Cjing3D
{

namespace EventSystem
{
	using CallbackFunc = std::function<void(const VariantArray&)>;

	Connection Register(EventType eventID, CallbackFunc func);
	void RegisterOnce(EventType eventID, CallbackFunc func);
	void Fire(EventType eventID, const VariantArray& variants);
}

}