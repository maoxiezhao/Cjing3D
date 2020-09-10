#include "engineWin32.h"
#include "helper\fileSystem.h"
#include "helper\profiler.h"
#include "helper\variant.h"
#include "platform\platform.h"
#include "core\jobSystem.h"
#include "core\settings.h"
#include "resource\resourceManager.h"
#include "renderer\renderer.h"
#include "renderer\presentConfig.h"
#include "scripts\luaContext.h"
#include "gui\guiStage.h"
#include "system\sceneSystem.h"
#include "audio\audio.h"
#include "audio\audioDeviceX.h"
#include "platform\systemEvent.h"
#include "platform\win32\gameWindowWin32.h"
#include "platform\win32\inputSystemWin32.h"
#include "renderer\RHI\d3d11\deviceD3D11.h"
#include "core\eventSystem.h"

namespace Cjing3D::Win32
{
	namespace
	{
		std::shared_ptr<Settings>         mSettings = nullptr;
		std::shared_ptr<JobSystem>        mJobSystem = nullptr;
		std::shared_ptr<InputManagerWin32>mInputManager = nullptr;
		std::shared_ptr<AudioManager>     mAudioManager = nullptr;
		std::shared_ptr<ResourceManager>  mResourceManager = nullptr;
		std::shared_ptr<GUIStage>         mGUIStage = nullptr;
		std::shared_ptr<LuaContext>       mLuaContext = nullptr;
		std::shared_ptr<RendererSystem>   mRendererSystem = nullptr;
		std::vector<std::shared_ptr<SubSystem>> mSubSystems;

		template<typename T>
		constexpr void CheckSubSystemType()
		{
			static_assert(std::is_base_of<SubSystem, T>::value, "The T must implement Subystem");
		}

		template<typename T, typename... Args>
		std::shared_ptr<T> RegisterSubSystem(Args&&... args)
		{
			CheckSubSystemType<T>();

			auto instant = CJING_MAKE_SHARED<T>();
			mSubSystems.push_back(instant);
			instant->Initialize(std::forward<Args>(args)...);
			return instant;
		}

		void ClearAllSystems()
		{
			for (auto it = mSubSystems.rbegin(); it != mSubSystems.rend(); it++) {
				(*it)->Uninitialize();
			}
			mSubSystems.clear();

			mSettings = nullptr;
			mJobSystem = nullptr;
			mResourceManager = nullptr;
			mRendererSystem = nullptr;
			mInputManager = nullptr;
			mAudioManager = nullptr;
			mGUIStage = nullptr;
			mLuaContext = nullptr;

			GetGlobalContext().Clear();
			GetGlobalContext().SetCurrentEngine(nullptr);
		}

		std::shared_ptr<GraphicsDevice> CreateGraphicsDeviceByType(RenderingDeviceType deviceType, const GameWindow& gameWindow,  FORMAT backBufferFormat, bool isDebug)
		{
			std::shared_ptr<GraphicsDevice> graphicsDevice = nullptr;
			switch (deviceType)
			{
			case RenderingDeviceType_D3D11:
				graphicsDevice = CJING_MAKE_SHARED<GraphicsDeviceD3D11>(gameWindow, backBufferFormat, isDebug);				graphicsDevice->Initialize();
				break;
			}
			return graphicsDevice;
		}
	}

	EngineWin32::EngineWin32(const std::shared_ptr<GameWindowWin32>& gameWindow, PresentConfig& config) :
		Engine(gameWindow, config),
		mGameWindowWin32(gameWindow)
	{
	}

	EngineWin32::~EngineWin32()
	{
	}

	void EngineWin32::Initialize()
	{

#ifdef CJING_DEBUG
		Profiler::GetInstance().Initialize();
		Profiler::GetInstance().SetProfileEnable(true);
		bool isDebuggin = true;
#else
		bool isDebuggin = false;
#endif
		// initialize file data
		if (!FileData::OpenData("", mAssetPath, mAssetName))
		{
			Debug::Die("No data file was found int the direcion:" + mAssetPath + " " + mAssetName);
			return;
		}

		// initialize subsystems
		GetGlobalContext().SetCurrentEngine(shared_from_this());

		// need initilize first
		mSettings		 = RegisterSubSystem<Settings>();
		mJobSystem	     = RegisterSubSystem<JobSystem>();

		auto graphicsDevice = CreateGraphicsDeviceByType(
			RenderingDeviceType_D3D11,
			*mGameWindowWin32,
			mPresentConfig.mBackBufferFormat,
			isDebuggin
		);
		Renderer::SetDevice(graphicsDevice);

		mResourceManager = RegisterSubSystem<ResourceManager>();							   GetGlobalContext().gResourceManager = mResourceManager;
		mInputManager    = RegisterSubSystem<InputManagerWin32>(*mGameWindowWin32);			   GetGlobalContext().gInputManager = mInputManager;
		mAudioManager    = RegisterSubSystem<AudioManager>(CJING_MEM_NEW(Audio::AudioDeviceX));GetGlobalContext().gAudioManager = mAudioManager;
		mRendererSystem  = RegisterSubSystem<RendererSystem>();		
		mGUIStage        = RegisterSubSystem<GUIStage>();									   GetGlobalContext().gGUIStage = mGUIStage;
		mLuaContext      = RegisterSubSystem<LuaContext>();									   GetGlobalContext().gLuaContext = mLuaContext;
	}

	void EngineWin32::Uninitialize()
	{
#ifdef CJING_DEBUG
		Profiler::GetInstance().Clear();
#endif
		ClearAllSystems();

		FileData::CloseData();

#ifdef CJING_DEBUG
		Profiler::GetInstance().Uninitialize();
#endif
	}

	void EngineWin32::SetAssetPath(const std::string& path, const std::string& name)
	{
		mAssetPath = path;
		mAssetName = name;
	}

	void EngineWin32::SetSystemEventQueue(const std::shared_ptr<EventQueue>& eventQueue)
	{
		mSystemEventQueue = eventQueue;
		mSystemConnection = eventQueue->Connect([this](const Event& event) {
			HandleSystemMessage(event);
		});
	}

	void EngineWin32::HandleSystemMessage(const Event& systemEvent)
	{
		if (systemEvent.Is<WindowCloseEvent>())
		{
			SetIsExiting(true);
		}
		else if (systemEvent.Is<InputTextEvent>())
		{
			const InputTextEvent* event = systemEvent.As<InputTextEvent>();

			Gui::GUIInputEvent e = {};
			e.type = Gui::GUI_INPUT_EVENT_TYPE_INPUT_TEXT;
			e.inputText = event->mInputText;
			mGUIStage->PushInputEvent(e);
		}
		else if (systemEvent.Is<RAWMOUSE>())
		{
			const RAWMOUSE* event = systemEvent.As<RAWMOUSE>();
			mInputManager->ProcessMouseEvent(*event);
		}
		else if (systemEvent.Is<RAWKEYBOARD>())
		{
			const RAWKEYBOARD* event = systemEvent.As<RAWKEYBOARD>();
			mInputManager->ProcessKeyboardEvent(*event);
		}
		else if (systemEvent.Is<ViewResizeEvent>())
		{
			const ViewResizeEvent* event = systemEvent.As<ViewResizeEvent>();
			VariantArray variants;
			variants.push_back(Variant(event->width));
			variants.push_back(Variant(event->height));
			EventSystem::Fire(EVENT_RESOLUTION_CHANGE, variants);
		}
	}

	void EngineWin32::DoSystemEvents()
	{
		mSystemEventQueue->FireEvents();
	}

	std::shared_ptr<JobSystem> EngineWin32::GetJobSystem()
	{
		return mJobSystem;
	}

	std::shared_ptr<Settings> EngineWin32::GetSettings()
	{
		return mSettings;
	}

	std::shared_ptr<InputManager> EngineWin32::GetInputManager()
	{
		return mInputManager;
	}

	std::shared_ptr<AudioManager> EngineWin32::GetAudioManager()
	{
		return mAudioManager;
	}

	std::shared_ptr<ResourceManager> EngineWin32::GetResourceMangaer()
	{
		return mResourceManager;
	}

	std::shared_ptr<GUIStage> EngineWin32::GetGUIStage()
	{
		return mGUIStage;
	}

	std::shared_ptr<LuaContext> EngineWin32::GetLuaContext()
	{
		return mLuaContext;
	}
}