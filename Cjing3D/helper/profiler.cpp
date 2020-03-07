#include "profiler.h"

namespace Cjing3D
{
namespace {
	bool OnOptickStateChanged(Optick::State::Type state)
	{
		if (state == Optick::State::DUMP_CAPTURE)
		{
			Optick::AttachSummary("Version", "v2.0");
			Optick::AttachSummary("Build", __DATE__ " " __TIME__);
		}
		return true;
	}
}

Profiler::~Profiler()
{
}

void Profiler::Initialize()
{
	OPTICK_SET_MEMORY_ALLOCATOR(
		[](size_t size) -> void* { return operator new(size); },
		[](void* p) { operator delete(p); },
		[]() {}
	);

	OPTICK_SET_STATE_CHANGED_CALLBACK(OnOptickStateChanged);
}

void Profiler::Uninitialize()
{
	OPTICK_SHUTDOWN();
}

void Profiler::BeginFrame()
{
	if (!IsProfileEnable()) {
		return;
	}
}

void Profiler::EndFrame()
{
	if (!IsProfileEnable()) {
		return;
	}
}

void Profiler::BeginBlock(const std::string & name)
{
	if (!IsProfileEnable()) {
		return;
	}
}

void Profiler::EndBlock()
{
	if (!IsProfileEnable()) {
		return;
	}
}

Profiler::Profiler()
{
}

}