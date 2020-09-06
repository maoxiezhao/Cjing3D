#pragma once

#include "definitions\definitions.h"
#include "app\gameComponent.h"

namespace Cjing3D
{
	class GameTest : public GameComponent
	{
	public:
		GameTest(const std::shared_ptr<Engine>& engine);
		virtual ~GameTest();

		void Initialize()override;
	};

}