#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\RHI\device.h"
#include "renderer\RHI\rhiResource.h"
#include "core\globalContext.hpp"

namespace Cjing3D {

	class RenderPath
	{
	public:
		RenderPath() {};
		virtual ~RenderPath() {};

		virtual void Initialize() {}
		virtual void Uninitialize() {}
		virtual void Start() {};
		virtual void Stop() {};
		virtual void Update(F32 dt) {};
		virtual void FixedUpdate() {};
		virtual void Render() {};
		virtual void Compose() {};
	};

}