#pragma once

#include "renderer\renderableCommon.h"

namespace Cjing3D
{
	class Component
	{
	public:
		Component();
		~Component();

	};

	using Component = std::shared_ptr<Component>;

}