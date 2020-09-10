#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\renderer.h"

namespace Cjing3D
{
	// RenderPass 在这里被定义为一个自洽的渲染行为,对于简单的渲染方法直接
	// 写在Renderer中，而相对复杂且自洽的绘制行为可以定义为RenderPass,例如地形、海洋

	class RenderPass
	{
	public:
		RenderPass() {};
		virtual ~RenderPass() {}

		virtual void Initialize() {};
		virtual void Uninitialize() {};
		virtual void UpdatePerFrameData(F32 deltaTime) {};
		virtual void RefreshRenderData(CommandList cmd) {};
		virtual void Render(CommandList cmd) {};
	};
}