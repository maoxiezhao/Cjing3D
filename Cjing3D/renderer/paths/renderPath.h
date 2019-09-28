#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\renderTarget.h"
#include "core\systemContext.hpp"

namespace Cjing3D {

	class Renderer;

	class RenderPath
	{
	public:
		RenderPath(Renderer& renderer);
		virtual ~RenderPath();

		virtual void Initialize() = 0;
		virtual void Uninitialize() = 0;

		virtual void Start() {};
		virtual void Stop() {};
		virtual void Update(F32 dt) {};
		virtual void Render() {};
		virtual void Compose() {};

	protected:
		Renderer& mRenderer;
		bool mIsInitialized;
	};

}