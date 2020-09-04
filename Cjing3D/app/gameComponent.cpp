#include "gameComponent.h"
#include "helper\fileSystem.h"
#include "helper\profiler.h"
#include "helper\enumInfo.h"
#include "platform\platform.h"
#include "platform\gameWindow.h"
#include "core\jobSystem.h"
#include "core\settings.h"
#include "input\InputSystem.h"
#include "resource\resourceManager.h"
#include "renderer\renderer.h"
#include "scripts\luaContext.h"
#include "gui\guiStage.h"
#include "system\sceneSystem.h"
#include "audio\audio.h"
#include "audio\audioDeviceX.h"
#include "renderer\presentConfig.h"

namespace Cjing3D
{

	GameComponent::GameComponent(const std::shared_ptr<Engine>& engine) :
		mEngine(engine),
		mGameWindow(engine->GetGameWindow())
	{
		mLuaContext   = mEngine->GetLuaContext();
		mGUIStage     = mEngine->GetGUIStage();
		mInputManager = mEngine->GetInputManager();
	}

	GameComponent::~GameComponent()
	{
	}

	void GameComponent::Initialize()
	{
		if (mIsInitialized == true) {
			return;
		}
		// initialize main scene
		Scene::Initialize();

		// start game
		GetGlobalContext().StartTimer();
		mDeltaTimeAccumulator = 0;

		// start game
		OnLoad();

		mIsInitialized = true;
	}

	void GameComponent::Uninitialize()
	{
		if (!mIsInitialized == true) {
			return;
		}

		// stop game
		OnUnLoad();

		mCurrentRenderPath.reset();
		mNextRenderPath.reset();

		// uninitialize sub systems
		Scene::GetScene().Clear();
		Scene::Uninitialize();

		GetGlobalContext().StopTimer();
		mIsInitialized = false;
	}

	void GameComponent::SetRenderPath(const std::shared_ptr<RenderPath>& renderPath)
	{
		mNextRenderPath = renderPath;
	}

	void GameComponent::Tick()
	{
		PROFILER_BEGIN_FRAME();

		GetGlobalContext().UpdateTimer();

		const auto& presentConfig = mEngine->GetPresentConfig();
		U32 targetFrameRate = presentConfig.mTargetFrameRate;
		U32 isLockFrameRate = presentConfig.mIsLockFrameRate;

		F32 deltaTime = GetGlobalContext().GetDelatTime();
		if (mGameWindow->IsWindowActive())
		{
			const F32 dt = isLockFrameRate ? (1.0f / targetFrameRate) : deltaTime;
			if (!mIsSkipFrame)
			{
				DoSystemEvents();
				FixedUpdate();
			}
			else
			{
				mDeltaTimeAccumulator += dt;

				// 某些情况会触发超时操作，此时需要重置计数
				if (mDeltaTimeAccumulator > 8) {
					mDeltaTimeAccumulator = 0;
				}

				const F32 targetRateInv = 1.0f / targetFrameRate;
				while (mDeltaTimeAccumulator >= targetRateInv)
				{
					DoSystemEvents();
					FixedUpdate();
					mDeltaTimeAccumulator -= targetRateInv;
				}
			}
			Update(dt);
			UpdateInput(dt);

			PreRender();
			Render();
			PostRender();
		}
		else
		{
			mDeltaTimeAccumulator = 0;
			UpdateInput(deltaTime);
		}

		Compose();
		EndFrame();
		PROFILER_END_FRAME();
	}

	void GameComponent::FixedUpdate()
	{
		PROFILER_BEGIN_CPU_BLOCK("FixedUpdate");
		mLuaContext->FixedUpdate();
		mGUIStage->FixedUpdate();

		if (mCurrentRenderPath != nullptr) {
			mCurrentRenderPath->FixedUpdate();
		}
		PROFILER_END_BLOCK();
	}

	void GameComponent::Update(F32 deltaTime)
	{
		if (mNextRenderPath != nullptr && mNextRenderPath != mCurrentRenderPath)
		{
			if (mCurrentRenderPath != nullptr) 
			{
				mCurrentRenderPath->Stop();
				mCurrentRenderPath = nullptr;
			}

			mCurrentRenderPath = mNextRenderPath;
			mNextRenderPath = nullptr;

			if (mCurrentRenderPath != nullptr)
			{
				mCurrentRenderPath->Start();
			}
		}

		PROFILER_BEGIN_CPU_BLOCK("Update");
		mLuaContext->Update(deltaTime);
		mGUIStage->Update(deltaTime);

		if (mCurrentRenderPath != nullptr) {
			mCurrentRenderPath->Update(deltaTime);
		}
		PROFILER_END_BLOCK();
	}

	void GameComponent::UpdateInput(F32 deltaTime)
	{
		PROFILER_BEGIN_CPU_BLOCK("UpdateInput");
		mInputManager->Update(deltaTime);
		PROFILER_END_BLOCK();
	}

	void GameComponent::Render()
	{
		PROFILER_BEGIN_CPU_BLOCK("Render");
		if (mCurrentRenderPath != nullptr) {
			mCurrentRenderPath->Render();
		}
		PROFILER_END_BLOCK();
	}

	void GameComponent::Compose()
	{
		PROFILER_BEGIN_CPU_BLOCK("Compose");

		auto& device = Renderer::GetDevice();
		CommandList cmd = device.GetCommandList();

		device.PresentBegin(cmd);
		if (mCurrentRenderPath != nullptr) {
			mCurrentRenderPath->Compose(cmd);
		}
		device.PresentEnd(cmd);

		PROFILER_END_BLOCK();
	}

	void GameComponent::EndFrame()
	{
		PROFILER_BEGIN_CPU_BLOCK("EndFrame");

		mLuaContext->GC();
		Renderer::EndFrame();

		PROFILER_END_BLOCK();
	}

	void GameComponent::OnLoad()
	{
		mLuaContext->StartMainScript();
	}

	void GameComponent::OnUnLoad()
	{
		mLuaContext->StopMainScript();
	}

	void GameComponent::DoSystemEvents()
	{
		PROFILER_BEGIN_CPU_BLOCK("DoSystemEvents");
		mEngine->DoSystemEvents();
		PROFILER_END_BLOCK();
	}
}