#pragma once

#include "definitions\definitions.h"
#include "app\gameComponent.h"

namespace Cjing3D
{
	class GameEditor : public GameComponent
	{
	public:
		GameEditor(const std::shared_ptr<Engine>& engine);
		virtual ~GameEditor();

		void Initialize()override;
		void Uninitialize()override;
		void FixedUpdate()override;
	};

}