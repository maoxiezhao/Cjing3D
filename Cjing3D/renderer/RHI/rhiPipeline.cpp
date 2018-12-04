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

	void Pipeline::SetSample()
	{
	}

	void Pipeline::SetRenderTarget()
	{
	}

	void Pipeline::SetConstantBuffer(ConstantBuffer & buffer, U32 slot, BufferScope bufferScope)
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