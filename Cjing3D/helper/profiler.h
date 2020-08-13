#pragma once

#include <unordered_map>

#include "common\common.h"
#include "optick.h"

namespace Cjing3D
{

class Profiler
{
public:
	static Profiler& GetInstance() {
		static Profiler profiler;
		return profiler;
	}

	~Profiler();
	Profiler(const Profiler& rhs) = delete;
	Profiler& operator=(const Profiler& rhs) = delete;

	void Initialize();
	void Uninitialize();
	void Clear();

	void BeginFrame();
	void EndFrame();
	void BeginCPUBlock(const StringID& name);
	void BeginGPUBlock(const StringID& name);
	void EndBlock();

	void SetProfileEnable(bool enable) { mProfileEnable = enable; };
	bool IsProfileEnable()const { return mProfileEnable; }

	F32 GetCPUBlockDeltaTime(const StringID& name);
	F32 GetGPUBlockDeltaTime(const StringID& name);

	std::string GetCPUProfileString()const;
	std::string GetGPUProfileString()const;

private:
	Profiler();
	
	bool mProfileEnable = false;
};

// profile with optick
#define PROFILER_OPTICK_FRAME    OPTICK_FRAME
#define PROFILER_OPTICK_EVENT    OPTICK_EVENT
#define PROFILER_OPTICK_CATEGORY OPTICK_CATEGORY

// profile 
#define PROFILER_BEGIN_FRAME() Profiler::GetInstance().BeginFrame();
#define PROFILER_END_FRAME() Profiler::GetInstance().EndFrame();
#define PROFILER_BEGIN_CPU_BLOCK(name) Profiler::GetInstance().BeginCPUBlock(StringID(name));
#define PROFILER_BEGIN_GPU_BLOCK(name) Profiler::GetInstance().BeginGPUBlock(StringID(name));
#define PROFILER_END_BLOCK() Profiler::GetInstance().EndBlock();
#define PROFILER_BEGIN_CPU_GPU_BLOCK(name)  Profiler::GetInstance().BeginGPUBlock(StringID(std::string("GPU_") + name));Profiler::GetInstance().BeginCPUBlock(StringID(std::string("CPU_") + name));
#define PROFILER_END_CPU_GPU_BLOCK() Profiler::GetInstance().EndBlock();Profiler::GetInstance().EndBlock();
}