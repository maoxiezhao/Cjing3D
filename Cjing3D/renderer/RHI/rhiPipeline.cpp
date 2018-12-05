#include "renderer\RHI\rhiPipeline.h"

namespace Cjing3D
{

	Pipeline::Pipeline(GraphicsDevice & device) :
		mDevice(device),
		mCullModeDirty(false),
		mFillModeDirty(false),
		mPrimitiveTopologyDirty(false)
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

		mSamplerState.clear();
		mSamplerStateDirty = false;
	}

	void Pipeline::SetViewport(ViewPort viewport)
	{
	}

	void Pipeline::SetVertexShader(VertexShaderPtr shader)
	{
	}

	void Pipeline::SetPixelShader(PixelShaderPtr shader)
	{
	}

	void Pipeline::SetTexture()
	{
	}

	void Pipeline::SetSampler(std::shared_ptr<SamplerState> sampler, U32 slot, SHADERSTAGES stage)
	{
	}

	void Pipeline::SetConstantBuffer(std::shared_ptr<ConstantBuffer> buffer, U32 slot, SHADERSTAGES stage)
	{
	}

	void Pipeline::SetRenderTarget()
	{
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