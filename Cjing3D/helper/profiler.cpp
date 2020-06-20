#include "profiler.h"
#include "timer.h"
#include "renderer\renderer.h"

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
		ProfilerType_GPU
	};
	struct ProfileBlock
	{
		ProfileType type;
		std::string name;
		F32 time;

		Timer cpuBegin;
		Timer cpuEnd;

		Renderer::GPUQueryHandler<4> gpuBegin;
		Renderer::GPUQueryHandler<4> gpuEnd;

		bool IsCPUBlock()const { return type == ProfilerType_CPU; }
		bool IsGPUBlock()const { return type == ProfilerType_GPU; }
	};
	std::unordered_map<U32, ProfileBlock> currentBlocks;
	std::stack<U32> recordedBlock;
	std::vector<U32> blockQueue;

	// 查询GPU时钟频率时需要调用BeginQuery/EndQuery
	Renderer::GPUQueryHandler<4> disjoint;
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
	disjoint.Uninitialize();
	currentBlocks.clear();

	OPTICK_SHUTDOWN();
}

void Profiler::BeginFrame()
{
	if (!IsProfileEnable()) {
		return;
	}

	if (!disjoint.IsInitialized()) 
	{
		GPUQueryDesc desc = {};
		desc.mGPUQueryType = GPU_QUERY_TYPE_TIMESTAMP_DISJOINT;
		disjoint.Initialize(desc);
	}

	// 查询GPU时钟频率时需要调用BeginQuery/EndQuery
	Renderer::GetDevice().BeginQuery(*disjoint.GetQueryForGPU());
	Renderer::GetDevice().EndQuery(*disjoint.GetQueryForGPU());

	// begin frame
	BeginCPUBlock(STRING_ID(CPU_FRAME));
	BeginGPUBlock(STRING_ID(GPU_FRAME));
}

void Profiler::EndFrame()
{
	if (!IsProfileEnable()) {
		return;
	}

	// end gpu frame block
	EndBlock();
	// end cpu frame block
	EndBlock();

	// read gpu timesteamp frequency
	U64 gpuTimestampFrequency = 0;
	GPUQuery* disjointQuery = disjoint.GetQueryForCPU();
	if (disjointQuery != nullptr)
	{
		GPUQueryResult result;
		while (Renderer::GetDevice().ReadQuery(*disjointQuery, result) == S_FALSE);
		gpuTimestampFrequency = result.mTimetampFrequency;
	}

	// calculate block time
	for (auto& kvp : currentBlocks)
	{
		ProfileBlock& block = kvp.second;
		block.time = 0;
		if (block.IsCPUBlock())
		{
			block.time = (F32)(block.cpuEnd.GetRecordedTimeStamp() - block.cpuBegin.GetRecordedTimeStamp());
		}
		else
		{
			GPUQuery* beginQuery = block.gpuBegin.GetQueryForCPU();
			GPUQuery* endQuery = block.gpuEnd.GetQueryForCPU();
			if (beginQuery != nullptr && endQuery != nullptr)
			{
				GPUQueryResult beginResult, endResult;
				while (Renderer::GetDevice().ReadQuery(*beginQuery, beginResult) == S_FALSE);
				while (Renderer::GetDevice().ReadQuery(*endQuery, endResult) == S_FALSE);

				block.time = std::abs(F32(endResult.mTimestamp - beginResult.mTimestamp) / (F32)gpuTimestampFrequency * 1000.0f);
			}
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

	U32 hashValue = name.HashValue();
	if (currentBlocks.find(hashValue) == currentBlocks.end())
	{
		auto it = currentBlocks.emplace(hashValue, ProfileBlock());
		ProfileBlock& cpuBlock = it.first->second;

		cpuBlock.type = ProfilerType_GPU;
		cpuBlock.name = name.GetString();
		cpuBlock.time = 0;

		GPUQueryDesc desc = {};
		desc.mGPUQueryType = GPU_QUERY_TYPE_TIMESTAMP;
		cpuBlock.gpuBegin.Initialize(desc);
		cpuBlock.gpuEnd.Initialize(desc);

		blockQueue.push_back(hashValue);
	}
	recordedBlock.push(hashValue);

	// 获取时间戳只要只执行QueryEnd即可
	auto& gpuBegin = currentBlocks[hashValue].gpuBegin;
	Renderer::GetDevice().EndQuery(*gpuBegin.GetQueryForGPU());
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
		ProfileBlock& block = it->second;
		if (block.IsCPUBlock())
		{
			block.cpuEnd.RecordDeltaTime();
		}
		else
		{
			// 获取时间戳只要只执行QueryEnd即可
			auto& gpuEnd = block.gpuEnd;
			Renderer::GetDevice().EndQuery(*gpuEnd.GetQueryForGPU());
		}
	}
}

F32 Profiler::GetCPUBlockDeltaTime(const StringID& name)
{
	F32 deltaTime = 0.0f;
	U32 hashValue = name.HashValue();

	auto it = currentBlocks.find(hashValue);
	if (it != currentBlocks.end() && it->second.IsCPUBlock()) {
		deltaTime = it->second.time;
	}

	return deltaTime;
}

F32 Profiler::GetGPUBlockDeltaTime(const StringID& name)
{
	F32 deltaTime = 0.0f;
	U32 hashValue = name.HashValue();

	auto it = currentBlocks.find(hashValue);
	if (it != currentBlocks.end() && it->second.IsGPUBlock()) {
		deltaTime = it->second.time;
	}

	return deltaTime;
}

std::string Profiler::GetCPUProfileString()const
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

std::string Profiler::GetGPUProfileString() const
{
	if (!IsProfileEnable()) {
		return "";
	}

	std::stringstream profileStr("");
	profileStr.precision(2); // .2f
	profileStr << "GPU profiler blocks:" << std::endl << "----------------------------" << std::endl;

	// GPU Blocks
	for (U32 hashValue : blockQueue)
	{
		ProfileBlock& block = currentBlocks[hashValue];
		if (block.IsGPUBlock())
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