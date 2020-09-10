
#include "generator.h"
#include "helper\debug.h"
#include "helper\fileSystem.h"
#include "helper\debug.h"

#include <iostream>
#include <Windows.h>

//#define __BINDER_DEBUG__
//#define __BINDER_DEBUG_SYSTEM_PAUSE__
//#define USE_CPLUSPLUS_PARSE

// 是否开启自动分析头文件生成lua绑定代码
#define __ENABLE_GENERATE_LUA_BINDING_CODES__

int main(int argc, char** argv)
{
#ifndef __ENABLE_GENERATE_LUA_BINDING_CODES__
	return 0;
#endif

#ifdef __BINDER_DEBUG_SYSTEM_PAUSE__
	system("Pause");
#endif

#ifndef __BINDER_DEBUG__
	if (argc < 4) {
		Cjing3D::Debug::Error("Error argc" + std::to_string(argc));
		return 0;
	}
#endif

	std::string sourcePath = "./../LuaBinder";
	std::string generatePath = "./../LuaBinder";

#ifndef __BINDER_DEBUG__
	std::string mode = argv[1];
	if (mode.find("-bind") == std::string::npos)
	{
		Cjing3D::Debug::Error("Error mode:" + mode);
		return 0;
	}

	sourcePath = argv[2];
	if (sourcePath == "") {
		Cjing3D::Debug::Error("Error sourcePath:" + sourcePath);
		return 0;
	}

	generatePath = argv[3];
	if (generatePath == "") {
		Cjing3D::Debug::Error("Error generatePath:" + generatePath);
		return 0;
	}
#endif

	Cjing3D::Debug::SetDebugConsoleEnable(true);

	if (!Cjing3D::FileData::OpenData("", sourcePath)) {
		Cjing3D::Debug::Error("No data file was found int the direction:" + sourcePath);
		return 0;
	}

	Cjing3D::FileData::SetGeneratingWriteDir(generatePath);

	Cjing3D::LuaBindingsGenerator generator;
	generator.ParseAllHeader("");
	generator.GenerateSource("luabind_registers");

	return 0;
}

