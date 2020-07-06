#pragma once

#include "definitions\definitions.h"
#include "core\gameComponent.h"
#include "utils\signal\connectionList.h"

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


	};

}