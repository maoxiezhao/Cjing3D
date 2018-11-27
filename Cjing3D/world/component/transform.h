#pragma once

#include "world\component\component.h"
#include "core\systemContext.hpp"

namespace Cjing3D {

	class Transform : public Component
	{
	public:
		Transform(SystemContext& gameContext);
		virtual ~Transform();

		virtual void Initialize();
		virtual void Uninitialize();
		virtual void Update();
	};

}