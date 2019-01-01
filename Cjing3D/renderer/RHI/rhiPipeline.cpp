#include "renderer\RHI\rhiPipeline.h"

namespace Cjing3D
{
	Pipeline::Pipeline(GraphicsDevice & device) :
		mDevice(device),
		mCullModeDirty(false),
		mFillModeDirty(false),
		mPrimitiveTopologyDirty(false),
		mRenderTargetViewDirty(false)
	{
	}

	Pipeline::~Pipeline()
	{
	}

	void Pipeline::Bind()
	{

	}

	void Pipeline::Clear()
	{
		mConstantBuffers.clear();
		mConstantBufferDirty = false;;

		mCullMode = CULL_NONE;
		mCullModeDirty = false;

		mFillMode = FILL_SOLID;
		mFillModeDirty = false;;

		mPrimitiveTopology = PRIMITIVE_TOPOLOGY::TRIANGLELIST;
		mPrimitiveTopologyDirty = false;

		mViewportDirty = false;

		mShaderInfoState.Clear();
		mShaderInfoStateDirty = false;

		mSamplerState.clear();
		mSamplerStateDirty = false;

		mTextures.clear();
		mTextureDirty = false;

		mRenderTargetViews.clear();
		mRenderTargetViewDirty = false;
	}

	void Pipeline::SetViewport(ViewPort viewport)
	{
		mViewport = viewport;
		mViewportDirty = true;
	}

	void Pipeline::SetVertexShader(VertexShaderInfo shader)
	{
		auto& vertexShader = shader.mVertexShader;
		if (vertexShader != mShaderInfoState.mVertexShader)
		{
			mShaderInfoState.mVertexShader = vertexShader;
			mShaderInfoStateDirty = true;
		}

		auto& layout = shader.mInputLayout;
		SetInputLayout(layout);
	}

	void Pipeline::SetPixelShader(PixelShaderPtr shader)
	{
		
	}

	void Pipeline::SetInputLayout(std::shared_ptr<InputLayout> layout)
	{
		if (layout != mShaderInfoState.mInputLayout)
		{
			mShaderInfoState.mInputLayout = layout;
			mShaderInfoStateDirty = true;
		}
	}

	void Pipeline::SetTexture(RhiTexture2DPtr texture, U32 slot, SHADERSTAGES stage)
	{
		if (texture != nullptr)
		{
			BindingTexture bindingTexture;
			bindingTexture.data = texture;
			bindingTexture.slot = slot;
			bindingTexture.stage = stage;
			
			mTextures.push_back(bindingTexture);
			mTextureDirty = true;
		}
	}

	void Pipeline::SetRenderTargets(const std::vector<RhiTexture2DPtr>& textures, RhiTexture2D * depthStencilTexture)
	{
		if (textures.empty()) {
			return;
		}

		mRenderTargetViews.clear();

		for (const auto& texture : textures)
		{
			if (texture == nullptr) {
				continue;
			}
			mRenderTargetViews.push_back(texture);
		}

		mDepthStencilTexture = depthStencilTexture;
		mRenderTargetViewDirty = true;
	}

	void Pipeline::SetSampler(std::shared_ptr<SamplerState> sampler, U32 slot, SHADERSTAGES stage)
	{
		if (sampler != nullptr)
		{
			BindingSamplerState bindingSamplerState;
			bindingSamplerState.data = sampler;
			bindingSamplerState.slot = slot;
			bindingSamplerState.stage = stage;

			mSamplerState.push_back(bindingSamplerState);
			mSamplerStateDirty = true;
		}
	}

	void Pipeline::SetConstantBuffer(std::shared_ptr<GPUBuffer> buffer, U32 slot, SHADERSTAGES stage)
	{
		if (buffer != nullptr)
		{
			BindingConstantBuffer bndingConstantBuffer;
			bndingConstantBuffer.data = buffer;
			bndingConstantBuffer.slot = slot;
			bndingConstantBuffer.stage = stage;

			mConstantBuffers.push_back(bndingConstantBuffer);
			mConstantBufferDirty = true;
		}
	}

	void Pipeline::SetCullMode(CullMode cullMode)
	{
		if (mCullMode == cullMode) {
			return;
		}

		mCullMode = cullMode;
		mCullModeDirty = true;
	}

	void Pipeline::SetFillMode(FillMode fillMode)
	{
		if (mFillMode == fillMode) {
			return;
		}

		mFillMode = fillMode;
		mFillModeDirty = true;
	}

	void Pipeline::SetPrimitiveTopology(PRIMITIVE_TOPOLOGY topology)
	{
		mPrimitiveTopology = topology;
	}

}