#include "gameComponent.h"
#include "engine.h"
#include "helper\profiler.h"
#include "renderer\renderer.h"
#include "renderer\RHI\deviceD3D11.h"

namespace Cjing3D {

void GameComponent::Initialize(Engine& engine)
{
	InitializeImpl();

	// initialize render
	HWND window = engine.GetMainWindow().GetHwnd();
	Renderer::GetInstance().Initialize(
		new GraphicsDeviceD3D11(window, false)
	);
}

void GameComponent::Update(EngineTime time)
{
	UpdateImpl(time);
}

void GameComponent::Uninitialize()
{
	// uninitialize render
	Renderer::GetInstance().Uninitialize();

	UninitializeImpl();
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

	Profiler::GetInstance().EndFrame();
}

void GameComponent::Render()
{
	if (mRenderableComponent != nullptr) {
		mRenderableComponent->Render();
	}
}

}
