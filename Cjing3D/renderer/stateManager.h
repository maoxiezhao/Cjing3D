#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\renderer.h"
#include "renderer\renderResource.h"

namespace Cjing3D
{
	class StateManager
	{
	public:
		StateManager(Renderer& renderer);

		void Initialize();
		void Uninitialize();

		void GetShader();
		void GetBuffer();

	private:
		void LoadShader();
		void LoadBuffers();

	private:
		Renderer & mRenderer;

	};
}