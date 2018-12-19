#pragma once

#include "renderer\renderableCommon.h"
#include "resource\resource.h"

namespace Cjing3D
{
	class Material : public Resource
	{
	public:
		Material();
		~Material();

	};

	using MaterialPtr = std::shared_ptr<Material>;

}