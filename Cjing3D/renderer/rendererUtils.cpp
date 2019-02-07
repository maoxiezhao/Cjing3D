#include "rendererUtils.h"
#include "renderer\RHI\device.h"
#include "world\actor.h"

namespace Cjing3D
{
	DeferredMIPGenerator::DeferredMIPGenerator(GraphicsDevice & device):
		mDevice(device)
	{
	}

	void DeferredMIPGenerator::AddTexture(RhiTexture2DPtr texture)
	{
	}

	void DeferredMIPGenerator::UpdateMipGenerating()
	{
		// 处理每个mip生成任务
		for (auto& texture : mMipGenDeferredArray) {
			GenerateMaipChain(*texture, MIPGENFILTER::MIPGENFILTER_LINEAR);
		}
		mMipGenDeferredArray.clear();
	}

	void DeferredMIPGenerator::GenerateMaipChain(RhiTexture2D & texture, MIPGENFILTER filter)
	{
		auto desc = texture.GetDesc();

		mDevice.BindRenderTarget(0, nullptr, nullptr);
		switch (filter)
		{
		case MIPGENFILTER_POINT:
			break;
		case MIPGENFILTER_LINEAR:
			break;
		case MIPGENFILTER_LINEAR_MAXIMUM:
			break;
		case MIPGENFILTER_GAUSSIAN:
			break;
		default:
			break;
		}

		// generate mipmap by compute shader
		U32 width = desc.mWidth;
		U32 height = desc.mHeight;
		for (int i = 0; i < desc.mMipLevels; i++)
		{
			width = std::max((U32)1, width / 2);
			height = std::max((U32)1, height / 2);


		}
	}

	void RenderBatch::Init(Renderable& renderable)
	{
		// TODO
		mRenderable = &renderable;
	}

	void RenderBatch::Init(ECS::Entity objectEntity, ECS::Entity meshEntity)
	{
	}

	RenderBatch::RenderBatch():
		mRenderable(nullptr)
	{
	}

	RenderBatch::~RenderBatch()
	{
	}

}