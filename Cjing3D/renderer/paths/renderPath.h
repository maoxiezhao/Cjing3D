#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\RHI\device.h"
#include "renderer\RHI\rhiResource.h"
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
		virtual void Update(F32 dt);
		virtual void Render() {};
		virtual void Compose() {};
		virtual void ResizeBuffers() {}

	protected:
		Renderer& mRenderer;

	private:
		bool mIsBufferInitialized = false;
	};

}