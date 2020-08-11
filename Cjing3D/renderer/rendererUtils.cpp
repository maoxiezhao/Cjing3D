#include "rendererUtils.h"
#include "renderer\renderer.h"
#include "renderer\shaderLib.h"
#include "renderer\preset\renderPreset.h"

namespace Cjing3D
{
	DeferredMIPGenerator::DeferredMIPGenerator()
	{
	}

	void DeferredMIPGenerator::AddTexture(Texture2D& texture, MipGenerateOption option)
	{
		mMipGenDeferredArray.push_back(std::make_pair(&texture, option));
	}

	void DeferredMIPGenerator::UpdateMipGenerating()
	{
		// 处理每个mip生成任务
		for (const auto& it : mMipGenDeferredArray) {
			GenerateMipChain(*(it.first), MIPGENFILTER::MIPGENFILTER_LINEAR, it.second);
		}
		mMipGenDeferredArray.clear();
	}

	void DeferredMIPGenerator::GenerateMipChain(Texture2D & texture, MIPGENFILTER filter, MipGenerateOption option)
	{
		auto desc = texture.GetDesc();
		if (desc.mMipLevels <= 1) {
			return;
		}

		GraphicsDevice& device = Renderer::GetDevice();
		RenderPreset& renderPreset = Renderer::GetRenderPreset();

		// generate mipmap by compute shader
		bool isHDR = device.IsFormatUnorm(desc.mFormat);
		U32 dispatchZ = 1;
		if (desc.mMiscFlags & RESOURCE_MISC_TEXTURECUBE)
		{
			switch (filter)
			{
			case MIPGENFILTER_POINT:
				device.BeginEvent("GenerateMipChain-FilterPoint");
				device.BindComputeShader(renderPreset.GetComputeShader(isHDR ? ComputeShaderType_MipmapCubeGenerateUnorm : ComputeShaderType_MipmapCubeGenerate));
				device.BindSamplerState(SHADERSTAGES_CS, *renderPreset.GetSamplerState(SamplerStateID_PointClamp), SAMPLER_SLOT_0);

				break;
			case MIPGENFILTER_LINEAR:
				device.BeginEvent("GenerateMipChain-FilterLinear");
				device.BindComputeShader(renderPreset.GetComputeShader(isHDR ? ComputeShaderType_MipmapCubeGenerateUnorm : ComputeShaderType_MipmapCubeGenerate));
				device.BindSamplerState(SHADERSTAGES_CS, *renderPreset.GetSamplerState(SamplerStateID_LinearClamp), SAMPLER_SLOT_0);

				break;
			default:
				return;
			}
		}
		else
		{
			switch (filter)
			{
			case MIPGENFILTER_POINT:
				device.BeginEvent("GenerateMipChain-FilterPoint");
				device.BindComputeShader(renderPreset.GetComputeShader(isHDR ? ComputeShaderType_MipmapGenerateUnorm : ComputeShaderType_MipmapGenerate));
				device.BindSamplerState(SHADERSTAGES_CS, *renderPreset.GetSamplerState(SamplerStateID_PointClamp), SAMPLER_SLOT_0);
				dispatchZ = 6;
				break;
			case MIPGENFILTER_LINEAR:
				device.BeginEvent("GenerateMipChain-FilterLinear");
				device.BindComputeShader(renderPreset.GetComputeShader(isHDR ? ComputeShaderType_MipmapGenerateUnorm : ComputeShaderType_MipmapGenerate));
				device.BindSamplerState(SHADERSTAGES_CS, *renderPreset.GetSamplerState(SamplerStateID_LinearClamp), SAMPLER_SLOT_0);
				dispatchZ = 6;
				break;
			default:
				return;
			}
		}

		U32 currentWidth = desc.mWidth;
		U32 currentHeight = desc.mHeight;
		// 共生成miplevels-1次（不包含本身）
		for (int mipLevel = 0; mipLevel < desc.mMipLevels - 1; mipLevel++)
		{
			currentWidth = std::max(1u, currentWidth / 2);
			currentHeight = std::max(1u, currentHeight / 2);

			// 输出绑定的是下一级的纹理,必须先绑定UAV,不然上一级的ouput无法绑定到下一级的input
			device.BindUAV(&texture, 0, mipLevel + 1);

			// bind input and output
			// 输入绑定的是上一级的纹理
			device.BindGPUResource(SHADERSTAGES_CS, &texture, TEXTURE_SLOT_UNIQUE_0, mipLevel);

			// update constant buffer
			MipmapGenerateCB cb;
			cb.gMipmapGenResolution.x = currentWidth;
			cb.gMipmapGenResolution.y = currentHeight;
			cb.gMipmapInverseResolution.x = (1.0f / currentWidth);
			cb.gMipmapInverseResolution.y = (1.0f / currentHeight);
			GPUBuffer& mipgenBuffer = renderPreset.GetConstantBuffer(ConstantBufferType_MipmapGenerate);
			device.UpdateBuffer(mipgenBuffer, &cb, sizeof(MipmapGenerateCB));
			device.BindConstantBuffer(SHADERSTAGES_CS, mipgenBuffer, CB_GETSLOT_NAME(MipmapGenerateCB));

			device.Dispatch(
				(U32)((currentWidth + SHADER_POSTPROCESS_BLOCKSIZE - 1) / SHADER_POSTPROCESS_BLOCKSIZE),
				(U32)((currentHeight + SHADER_POSTPROCESS_BLOCKSIZE - 1) / SHADER_POSTPROCESS_BLOCKSIZE),
				dispatchZ
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

		// 用meshEntity作为hash sort
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