#pragma once

#include <unordered_map>

#include "common\common.h"
#include "optick\optick.h"

namespace Cjing3D
{

class Profiler
{
public:
	enum Profiler_TYPE
	{
		PROFILER_CPU,
		PROFILER_GUP
	};

	static Profiler& GetInstance() {
		static Profiler profiler;
		return profiler;
	}

	~Profiler();
	Profiler(const Profiler& rhs) = delete;
	Profiler& operator=(const Profiler& rhs) = delete;

	void Initialize();
	void Uninitialize();

	void BeginFrame();
	void EndFrame();
	void BeginBlock(const std::string& name);
	void EndBlock();

	void SetProfileEnable(bool enable) { mProfileEnable = enable; };
	bool IsProfileEnable()const { return mProfileEnable; }

private:
	Profiler();
	
	struct Block
	{
		Profiler_TYPE type;
		std::string name;
		F32 time;
	};

	std::unordered_map<std::string, Block> mBlock;

	bool mProfileEnable = false;
};

#define PROFILER_BEGIN_FRAME() Profiler::GetInstance().BeginFrame();
#define PROFILER_END_FRAME() Profiler::GetInstance().EndFrame();
#define PROFILER_BEGIN_BLOCK(name) Profiler::GetInstance().BeginBlock(name);
#define PROFILER_END_BLOCK() Profiler::GetInstance().EndBlock();

// profile with optick
#define PROFILER_OPTICK_FRAME    OPTICK_FRAME
#define PROFILER_OPTICK_EVENT    OPTICK_EVENT
#define PROFILER_OPTICK_CATEGORY OPTICK_CATEGORY
}