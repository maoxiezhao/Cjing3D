#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\renderer.h"

namespace Cjing3D
{
	// RenderPass �����ﱻ����Ϊһ����Ǣ����Ⱦ��Ϊ,���ڼ򵥵���Ⱦ����ֱ��
	// д��Renderer�У�����Ը�������Ǣ�Ļ�����Ϊ���Զ���ΪRenderPass,������Ρ�����

	class RenderPass
	{
	public:
		RenderPass(Renderer& renderer) : mRenderer(renderer) {};
		virtual ~RenderPass() {}

		virtual void Initialize() {};
		virtual void Uninitialize() {};
		virtual void UpdatePerFrameData(F32 deltaTime) {};
		virtual void RefreshRenderData() {};
		virtual void Render() {};

	protected:
		Renderer& mRenderer;
	};
}