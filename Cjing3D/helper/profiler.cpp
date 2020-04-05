#include "profiler.h"
#include "timer.h"

#include <stack>
#include <sstream>

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

	enum ProfileType
	{
		ProfilerType_CPU,
		ProfilerType_GUP
	};
	struct ProfileBlock
	{
		ProfileType type;
		std::string name;
		F32 time;

		Timer cpuBegin;
		Timer cpuEnd;

		bool IsCPUBlock()const { return type == ProfilerType_CPU; }
	};
	std::unordered_map<U32, ProfileBlock> currentBlocks;
	std::stack<U32> recordedBlock;
	std::vector<U32> blockQueue;
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


	BeginCPUBlock(STRING_ID(CPU_FRAME));
}

void Profiler::EndFrame()
{
	if (!IsProfileEnable()) {
		return;
	}

	// end cpu frame block
	EndBlock();

	// calculate block time
	for (auto& kvp : currentBlocks)
	{
		ProfileBlock& block = kvp.second;
		if (block.IsCPUBlock())
		{
			block.time = (F32)(block.cpuEnd.GetRecordedTimeStamp() - block.cpuBegin.GetRecordedTimeStamp());
		}
	}
}

void Profiler::BeginCPUBlock(const StringID& name)
{
	if (!IsProfileEnable()) {
		return;
	}

	U32 hashValue = name.HashValue();
	if (currentBlocks.find(hashValue) == currentBlocks.end())
	{
		auto it = currentBlocks.emplace(hashValue, ProfileBlock());
		ProfileBlock& cpuBlock = it.first->second;

		cpuBlock.type = ProfilerType_CPU;
		cpuBlock.name = name.GetString();
		cpuBlock.time = 0;
		cpuBlock.cpuBegin.Start();
		cpuBlock.cpuEnd.Start();

		blockQueue.push_back(hashValue);
	}

	currentBlocks[hashValue].cpuBegin.RecordDeltaTime();
	recordedBlock.push(hashValue);
}

void Profiler::BeginGPUBlock(const StringID& name)
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

	if (recordedBlock.empty()) {
		return;
	}

	U32 hashValue = recordedBlock.top();
	recordedBlock.pop();

	auto it = currentBlocks.find(hashValue);
	if (it == currentBlocks.end())
	{
		Debug::CheckAssertion(0);
	}
	else
	{
		ProfileBlock& cpuBlock = it->second;
		cpuBlock.cpuEnd.RecordDeltaTime();
	}
}

std::string Profiler::GetProfileString()const
{
	if (!IsProfileEnable()) {
		return "";
	}

	std::stringstream profileStr("");
	profileStr.precision(2); // .2f
	profileStr << "Frame profiler blocks:" << std::endl << "----------------------------" << std::endl;

	// CPU Blocks
	for (U32 hashValue : blockQueue)
	{
		ProfileBlock& block = currentBlocks[hashValue];
		if (block.IsCPUBlock())
		{
			profileStr << block.name << ": " << std::fixed << block.time << " ms" << std::endl;
		}
	}

	return profileStr.str();
}

Profiler::Profiler()
{
}

}