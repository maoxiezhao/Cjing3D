#pragma once

#include "core\gameComponent.h"

namespace Cjing3D
{
	class TestGame : public GameComponent
	{
	public:
		TestGame();
		virtual ~TestGame();

		virtual void Setup();
		virtual void Initialize();
		virtual void Update(EngineTime time);
		virtual void Uninitialize();

	};

}