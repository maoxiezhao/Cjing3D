#pragma once

#include "definitions\definitions.h"
#include "core\gameComponent.h"

namespace Cjing3D
{
	class GameEditor : public GameComponent
	{
	public:
		GameEditor();
		virtual ~GameEditor();

		virtual void Initialize();
		virtual void Update(EngineTime time);
		virtual void FixedUpdate();
		virtual void Uninitialize();
		virtual void EndFrame();
	};

}