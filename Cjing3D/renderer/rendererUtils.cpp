#include "rendererUtils.h"
#include "renderer\renderer.h"
#include "renderer\shaderLib.h"
#include "renderer\rhiResourceManager.h"

namespace Cjing3D
{
	DeferredMIPGenerator::DeferredMIPGenerator()
	{
	}

	void DeferredMIPGenerator::AddTexture(Texture2D& texture)
	{
		mMipGenDeferredArray.push_back(&texture);
	}

	void DeferredMIPGenerator::UpdateMipGenerating()
	{
		// ����ÿ��mip��������
		for (auto texture : mMipGenDeferredArray) {
			GenerateMipChain(*texture, MIPGENFILTER::MIPGENFILTER_LINEAR);
		}
		mMipGenDeferredArray.clear();
	}

	void DeferredMIPGenerator::GenerateMipChain(Texture2D & texture, MIPGENFILTER filter)
	{
		auto desc = texture.GetDesc();
		if (desc.mMipLevels <= 1) {
			return;
		}

		GraphicsDevice& device = Renderer::GetDevice();
		ShaderLib& shaderLib = Renderer::GetShaderLib();
		RhiResourceManager& rhiResourceManager = Renderer::GetStateManager();

		// generate mipmap by compute shader
		switch (filter)
		{
		case MIPGENFILTER_POINT:
			device.BeginEvent("GenerateMipChain-FilterPoint");
			device.BindComputeShader(shaderLib.GetComputeShader(ComputeShaderType_MipmapGenerate));
			device.BindSamplerState(SHADERSTAGES_CS, *rhiResourceManager.GetSamplerState(SamplerStateID_PointClampGreater), SAMPLER_SLOT_0);

			break;
		case MIPGENFILTER_LINEAR:
			device.BeginEvent("GenerateMipChain-FilterLinear");
			device.BindComputeShader(shaderLib.GetComputeShader(ComputeShaderType_MipmapGenerate));
			device.BindSamplerState(SHADERSTAGES_CS, *rhiResourceManager.GetSamplerState(SamplerStateID_LinearClampGreater), SAMPLER_SLOT_0);

			break;
		default:
			return;
		}

		U32 currentWidth = desc.mWidth;
		U32 currentHeight = desc.mHeight;
		// ������miplevels-1�Σ�����������
		for (int mipLevel = 0; mipLevel < desc.mMipLevels - 1; mipLevel++)
		{
			currentWidth = std::max(1u, currentWidth / 2);
			currentHeight = std::max(1u, currentHeight / 2);

			// ����󶨵�����һ��������,�����Ȱ�UAV,��Ȼ��һ����ouput�޷��󶨵���һ����input
			device.BindUAV(&texture, 0, mipLevel + 1);

			// bind input and output
			// ����󶨵�����һ��������
			device.BindGPUResource(SHADERSTAGES_CS, texture, TEXTURE_SLOT_UNIQUE_0, mipLevel);

			// update constant buffer
			MipmapGenerateCB cb;
			cb.gMipmapGenResolution.x = currentWidth;
			cb.gMipmapGenResolution.y = currentHeight;
			cb.gMipmapInverseResolution.x = (1.0f / currentWidth);
			cb.gMipmapInverseResolution.y = (1.0f / currentHeight);
			GPUBuffer& mipgenBuffer = rhiResourceManager.GetConstantBuffer(ConstantBufferType_MipmapGenerate);
			device.UpdateBuffer(mipgenBuffer, &cb, sizeof(MipmapGenerateCB));
			device.BindConstantBuffer(SHADERSTAGES_CS, mipgenBuffer, CB_GETSLOT_NAME(MipmapGenerateCB));

			device.Dispatch(
				(U32)((currentWidth + SHADER_POSTPROCESS_BLOCKSIZE - 1) / SHADER_POSTPROCESS_BLOCKSIZE),
				(U32)((currentHeight + SHADER_POSTPROCESS_BLOCKSIZE - 1) / SHADER_POSTPROCESS_BLOCKSIZE),
				1
			);
		}

		device.UnBindUAVs(0, 1);
		device.UnbindGPUResources(TEXTURE_SLOT_UNIQUE_0, 1);
		device.EndEvent();
	}

	void RenderBatch::Init(ECS::Entity objectEntity, ECS::Entity meshEntity, F32 distance)
	{
		mObjectEntity = objectEntity;
		mMeshEntity = meshEntity;

		// ��meshEntity��Ϊhash sort
		mHash = (U32)(meshEntity) & 0x00FFFFFF;
		mHash |= (U32)(distance) & 0xFF;
	}

	RenderBatch::RenderBatch()
	{
	}

	RenderBatch::~RenderBatch()
	{
	}

}