#pragma once

#include "renderer\renderableCommon.h"

// need to refactor
#include "renderer\RHI\rhiResource.h"
#include "renderer\RHI\rhiShader.h"
#include "renderer\RHI\rhiBuffer.h"

namespace Cjing3D
{

struct BindingConstantBuffer
{
	void* const* buffer;
	U32 slot;
	BufferScope bufferScope;
};

class GraphicsDevice;

class Pipeline
{
public:
	Pipeline(GraphicsDevice& device);
	~Pipeline();

	void Bind();
	void Clear();

	void SetViewport(ViewPort viewport);

	void SetVertexShader(VertexShaderPtr shader);
	void SetPixelShader(PixelShaderPtr shader);

	void SetTexture();

	void SetSample();

	void SetRenderTarget();

	void SetConstantBuffer(ConstantBuffer& buffer, U32 slot, BufferScope bufferScope);

	void SetCullMode(CullMode cullMode);
	void SetFillMode(FillMode fillMode);

	void SetPrimitiveTopology(PRIMITIVE_TOPOLOGY topology);

private:
	GraphicsDevice& mDevice;

	std::vector<BindingConstantBuffer> mConstantBuffers;

	CullMode mCullMode;
	FillMode mFillMode;
	PRIMITIVE_TOPOLOGY mPrimitiveTopology;
	ViewPort mViewport;
};
}