#pragma once

#include "renderer\renderableComponent.h"
#include "helper\timer.h"

namespace Cjing3D
{
	class Engine;

	class GameComponent
	{
	public:
		GameComponent() {}
		virtual ~GameComponent() {}

		void Run(Timer& timer);
		void Render();

		void Initialize(Engine& engine);
		void Update(EngineTime time);
		void Uninitialize();

		RenderableComponentPtr GetRenderableComponent() {
			return mRenderableComponent;
		}

		void RequestRenderableComponent(RenderableComponentPtr component) {
			mRenderableComponent = component;
		}

	protected:
		virtual void InitializeImpl() = 0;
		virtual void UpdateImpl(EngineTime time) = 0;
		virtual void UninitializeImpl() = 0;

	private:
		RenderableComponentPtr mRenderableComponent;

	};
}