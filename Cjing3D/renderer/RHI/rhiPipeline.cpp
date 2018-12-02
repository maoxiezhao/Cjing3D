#include "renderer\RHI\rhiPipeline.h"

namespace Cjing3D
{

	Pipeline::Pipeline(GraphicsDevice & device) :
		mDevice(device)
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
	}

	void Pipeline::SetFillMode(FillMode fillMode)
	{
	}

}