#pragma once

#include "renderer\renderableCommon.h"

namespace Cjing3D
{
	class GraphicsDevice
	{
	public:
		GraphicsDevice();

		virtual void Initialize();
		virtual void Uninitialize();

	protected:
		bool mIsFullScreen;
	};
}