#pragma once

#include "common\definitions.h"
#include "utils\thread\spinLock.h"

#include <memory>
#include <functional>
#include <optional>

namespace Cjing3D
{
	namespace SingalImpl
	{
		template <typename FUNC>
		class SignalBody;
	}

	template <typename FUNC>
	class Signal;
}