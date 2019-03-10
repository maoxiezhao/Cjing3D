#include "renderImage.h"
#include "renderer\renderer.h"
#include "renderer\RHI\device.h"
#include "renderer\stateManager.h"
#include "renderer\shaderLib.h"

namespace Cjing3D
{
namespace RenderImage
{
	void Render(RhiTexture2D & texture, Renderer& renderer)
	{
		auto& device = renderer.GetDevice();
		auto& shaderLib = renderer.GetShaderLib();
		auto& stateManager = renderer.GetStateManager();

		device.BindGPUResource(SHADERSTAGES_PS, texture, TEXTURE_SLOT_0);
		device.BindSamplerState(SHADERSTAGES_PS, *stateManager.GetSamplerState(SamplerStateID_LinearClampGreater), SAMPLER_LINEAR_CLAMP_SLOT);
		device.BindShaderInfoState(shaderLib.GetImageShaderInfoState());

		device.Draw(3, 0);
	}
}
}

