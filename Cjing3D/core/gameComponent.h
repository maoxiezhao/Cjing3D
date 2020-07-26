#pragma once

#include "core\systemContext.hpp"
#include "helper\timer.h"

namespace Cjing3D
{
	class Engine;

	class GameComponent
	{
	public:
		GameComponent();
		virtual ~GameComponent();

		virtual void Setup();
		virtual void Initialize();
		virtual void FixedUpdate();
		virtual void Update(EngineTime time);
		virtual void PreRender();
		virtual void Uninitialize();

		void SetGameContext(SystemContext* systemContext) {
			mSystemContext = systemContext;
		}

		SystemContext* GetGameContext() {
			return mSystemContext;
		}

	private:
		SystemContext* mSystemContext;

	};
}