#include "renderImage.h"
#include "renderer\renderer.h"
#include "renderer\RHI\device.h"
#include "renderer\rhiResourceManager.h"
#include "renderer\shaderLib.h"
#include "renderer\pipelineStates\pipelineStateManager.h"

namespace Cjing3D
{
namespace RenderImage
{
	void Render(Texture2D & texture, ImageParams params)
	{
		auto& device = Renderer::GetDevice();
		auto& shaderLib = Renderer::GetShaderLib();
		auto& rhiResourceManager = Renderer::GetStateManager();
		auto& pipelineStateManager = Renderer::GetPipelineStateManager();

		device.BindSamplerState(SHADERSTAGES_PS, *rhiResourceManager.GetSamplerState(SamplerStateID_LinearClampGreater), SAMPLER_LINEAR_CLAMP_SLOT);
		device.BindGPUResource(SHADERSTAGES_PS, texture, TEXTURE_SLOT_0);

		if (params.IsFullScreenEnabled()) 
		{
			device.BindPipelineState(pipelineStateManager.GetImagePipelineState(params));
			device.Draw(3, 0);
		}
		else
		{
			XMMATRIX matrix =
				XMMatrixScaling(params.mScale[0] * params.mSize[0], params.mScale[1] * params.mSize[1], 1)
				* XMMatrixTranslation(params.mPos[0], params.mPos[1], 0)
				* device.GetScreenProjection();

			auto& buffer = rhiResourceManager.GetConstantBuffer(ConstantBufferType_Image);
			ImageCB cb;
			cb.gImageColor = XMConvert(params.mColor);

			for (int i = 0; i < 4; i++)
			{ 
				XMVECTOR v = XMVectorSet(params.mCorners[i][0], params.mCorners[i][1], 0, 1);
				v = XMVector2Transform(v, matrix);
				XMStoreFloat4(&cb.gImageCorners[i], v);
			}
			device.UpdateBuffer(buffer, &cb, sizeof(ImageCB));
			
			device.BindConstantBuffer(SHADERSTAGES_VS, buffer, CB_GETSLOT_NAME(ImageCB));
			device.BindConstantBuffer(SHADERSTAGES_PS, buffer, CB_GETSLOT_NAME(ImageCB));
			device.BindPipelineState(pipelineStateManager.GetImagePipelineState(params));

			device.Draw(4, 0);
		}
	}
}
}

