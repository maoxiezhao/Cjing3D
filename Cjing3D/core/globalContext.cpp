#include "globalContext.hpp"

namespace Cjing3D
{
	GlobalContext::~GlobalContext()
	{
		Clear();
	}

	void GlobalContext::Clear()
	{
		gSettings = nullptr;
		gJobSystem = nullptr;
		gResourceManager = nullptr;
		gInputManager = nullptr;
		gAudioManager = nullptr;
		gGUIStage = nullptr;
		gLuaContext = nullptr;
	}

	F32 GlobalContext::GetCurrentTime()
	{
		return F32(mTime.totalDeltaTime / 1000.0f);
	}

	F32 GlobalContext::GetDelatTime()
	{
		return F32(mTime.deltaTime / 1000.0f);
	}

	void GlobalContext::StartTimer()
	{
		mEngineTimer.Start();
	}

	void GlobalContext::UpdateTimer()
	{
		mTime = mEngineTimer.GetTime();
	}

	void GlobalContext::StopTimer()
	{
		mEngineTimer.Stop();
	}

	void GlobalContext::SetCurrentEngine(const std::shared_ptr<Engine>& engine)
	{
		mCurrentEngine = engine;
	}

	std::shared_ptr<Engine> GlobalContext::GetEngine()
	{
		return mCurrentEngine;
	}
}