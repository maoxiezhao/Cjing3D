#pragma once

#include "common\common.h"
#include "system\ecsSystem.h"

namespace Cjing3D
{
	class Scene;

	namespace ModelImporter
	{
		ECS::Entity ImportModelObj(const std::string& fileName, Scene& scene);
		ECS::Entity ImportModelGLTF(const std::string& fileName, Scene& scene);
	}
}