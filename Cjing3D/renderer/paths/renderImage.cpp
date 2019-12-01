#include "renderImage.h"
#include "renderer\renderer.h"
#include "renderer\RHI\device.h"
#include "renderer\stateManager.h"
#include "renderer\shaderLib.h"
#include "renderer\pipelineStateInfoManager.h"

namespace Cjing3D
{
namespace RenderImage
{

	void Render(RhiTexture2D & texture, ImageParams params, Renderer& renderer)
	{
		auto& device = renderer.GetDevice();
		auto& shaderLib = renderer.GetShaderLib();
		auto& stateManager = renderer.GetStateManager();
		auto& pipelineStateInfoManager = renderer.GetPipelineStateInfoManager();

		device.BindGPUResource(SHADERSTAGES_PS, texture, TEXTURE_SLOT_0);
		device.BindShaderInfoState(pipelineStateInfoManager.GetImagePipelineStateInfo(params));

		device.Draw(3, 0);
	}
}
}

