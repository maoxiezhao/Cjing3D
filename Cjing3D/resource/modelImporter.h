#pragma once

#include "common\common.h"
#include "system\ecsSystem.h"

namespace Cjing3D
{
	class SystemContext;

	namespace ModelImporter
	{
		ECS::Entity ImportModelObj(const std::string& fileName);
		ECS::Entity ImportModelGLTF(const std::string& fileName);
	}
}