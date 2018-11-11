#include "gameComponent.h"
#include "engine.h"
#include "helper\profiler.h"
#include "renderer\renderer.h"
#include "renderer\RHI\deviceD3D11.h"
#include "resource\resourceManager.h"
#include "world\world.h"

namespace Cjing3D {

GameComponent::GameComponent() :
	mRenderableComponent(nullptr),
	mGameContext(nullptr)
{
}

GameComponent::~GameComponent()
{
}

void GameComponent::Initialize(Engine& engine)
{
	BeforeInitializeImpl();

	mGameContext = std::make_unique<GameContext>();

	// initialize resource manager
	ResourceManager* resourceManager = new ResourceManager(*mGameContext);
	resourceManager->Initialize();
	mGameContext->RegisterGameSystem(resourceManager);

	// initialize render
	HWND window = engine.GetMainWindow().GetHwnd();
	Renderer* renderer = new Renderer(*mGameContext);
	mGameContext->RegisterGameSystem(renderer);
	renderer->Initialize(
		new GraphicsDeviceD3D11(window, false, true)
	);

	// initialize world
	World* world = new World(*mGameContext);
	mGameContext->RegisterGameSystem(world);
	world->Initialize();

	AfterInitializeImpl();
}

void GameComponent::Update(EngineTime time)
{
	UpdateImpl(time);
}

void GameComponent::Uninitialize()
{
	UninitializeImpl();

	// uninitialize world
	mGameContext->GetGameSystem<World>().Uninitialize();

	// uninitialize render
	mGameContext->GetGameSystem<Renderer>().Uninitialize();

	// uninitialize resource manager
	mGameContext->GetGameSystem<ResourceManager>().Uninitialize();
}

void GameComponent::Run(Timer& timer)
{
	Profiler::GetInstance().BeginFrame();
	auto time = timer.GetTime();

	// update
	Profiler::GetInstance().BeginBlock("Update");
	Update(time);
	Profiler::GetInstance().EndBlock();

	// render
	Profiler::GetInstance().BeginBlock("Render");
	Render();
	Profiler::GetInstance().EndBlock();

	// present
	mGameContext->GetGameSystem<Renderer>().Present();

	Profiler::GetInstance().EndFrame();
}

void GameComponent::Render()
{
	if (mRenderableComponent != nullptr) {
		mRenderableComponent->Render();
	}
}

}
