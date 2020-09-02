#pragma once

#include "common\common.h"
#include "platform\platform.h"
#include "app\engine.h"

namespace Cjing3D
{
	class RenderPath;
	class GameWindow;

	class GameComponent
	{
	public:
		GameComponent(const std::shared_ptr<Engine>& engine);
		virtual ~GameComponent();

		virtual void Initialize();
		virtual void Tick();
		virtual void Uninitialize();

		void SetRenderPath(const std::shared_ptr<RenderPath>& renderPath);
		bool GetIsExiting()const { return mEngine->GetIsExiting(); }

	protected:
		virtual void OnLoad();
		virtual void OnUnLoad();
		virtual void FixedUpdate();
		virtual void Update(F32 deltaTime);
		virtual void UpdateInput(F32 deltaTime);
		virtual void PreRender() {};
		virtual void Render();
		virtual void PostRender() {};
		virtual void Compose();
		virtual void EndFrame();
		virtual void DoSystemEvents();

	protected:
		bool mIsInitialized = false;
		bool mIsSkipFrame = true;
		F32 mDeltaTimeAccumulator = 0;

		std::shared_ptr<RenderPath> mCurrentRenderPath = nullptr;
		std::shared_ptr<RenderPath> mNextRenderPath = nullptr;
		std::shared_ptr<Engine>     mEngine = nullptr;
		std::shared_ptr<GameWindow> mGameWindow = nullptr;

		std::shared_ptr<InputManager> mInputManager = nullptr;
		std::shared_ptr<GUIStage>     mGUIStage = nullptr;
		std::shared_ptr<LuaContext>   mLuaContext = nullptr;
	};
}