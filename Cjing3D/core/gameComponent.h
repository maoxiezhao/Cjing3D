#pragma once

#include "core\globalContext.hpp"
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

		void SetGameContext(GlobalContext* globalContext) {
			mSystemContext = globalContext;
		}

		GlobalContext* GetGameContext() {
			return mSystemContext;
		}

	private:
		GlobalContext* mSystemContext;

	};
}