#pragma once

#include "common\common.h"
#include "core\subSystem.hpp"

namespace Cjing3D
{
	class GUIStage : public SubSystem
	{
	public:
		GUIStage(SystemContext& systemContext);
		~GUIStage();

		void Initialize();
		void Uninitialize();
		void Render();

		virtual void Update(F32 deltaTime);
	};
}