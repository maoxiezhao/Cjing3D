#pragma once

#include "definitions\definitions.h"
#include "app\gameComponent.h"
#include "renderer\paths\renderPath_tiledForward.h"

namespace Cjing3D
{
	class GameEditor : public GameComponent
	{
	public:
		GameEditor(const std::shared_ptr<Engine>& engine);
		virtual ~GameEditor();

		void FixedUpdate()override;
	};

}