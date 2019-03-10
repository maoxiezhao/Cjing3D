#pragma once

#include "common\common.h"

namespace Cjing3D
{
	class SystemContext;

	namespace ModelImporter
	{
		void ImportModelObj(const std::string& fileName, SystemContext& systemContext);
	}
}