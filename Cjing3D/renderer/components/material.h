#pragma once

#include "renderer\renderableCommon.h"

namespace Cjing3D
{
	class Material
	{
	public:
		Material();
		~Material();

	};

	using MaterialPtr = std::shared_ptr<Material>;

}