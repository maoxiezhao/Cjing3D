#pragma once

#include "world\component\component.h"
#include "core\gameContext.hpp"

namespace Cjing3D {

	class Transform : public Component
	{
	public:
		Transform(GameContext& gameContext);
		virtual ~Transform();

		virtual void Initialize();
		virtual void Uninitialize();
		virtual void Update();
	};

}