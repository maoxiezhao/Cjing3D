#pragma once

#include "core\gameComponent.h"
#include "game\scene\gameScene.h"

namespace CjingGame
{
	class MainGame : public Cjing3D::GameComponent
	{
	public:
		MainGame();
		virtual ~MainGame();

		virtual void Initialize();
		virtual void Update(Cjing3D::EngineTime time);
		virtual void FixedUpdate();
		virtual void Uninitialize();
	};

}