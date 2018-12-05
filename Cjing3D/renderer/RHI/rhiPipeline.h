#pragma once

#include "renderer\renderableCommon.h"

// need to refactor
#include "renderer\RHI\rhiResource.h"
#include "renderer\RHI\rhiShader.h"
#include "renderer\RHI\rhiBuffer.h"

namespace Cjing3D
{

template <typename T>
class BindingGraphicsResource
{
public:
	std::shared_ptr<T> data;
	U32 slot;
	SHADERSTAGES stage;
};

using BindingConstantBuffer = BindingGraphicsResource<ConstantBuffer>;
using BindingSamplerState = BindingGraphicsResource<SamplerState>;

//struct BindingConstantBuffer
//{
//	void* const* buffer;
//	U32 slot;
//	BufferScope bufferScope;
//};

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

	void SetRenderTarget();

	void SetSampler(std::shared_ptr<SamplerState> sampler, U32 slot, SHADERSTAGES stage);

	void SetConstantBuffer(std::shared_ptr<ConstantBuffer> buffer, U32 slot, SHADERSTAGES stage);

	void SetCullMode(CullMode cullMode);
	void SetFillMode(FillMode fillMode);

	void SetPrimitiveTopology(PRIMITIVE_TOPOLOGY topology);

private:
	GraphicsDevice& mDevice;

	std::vector<BindingConstantBuffer> mConstantBuffers;
	bool mConstantBufferDirty;

	CullMode mCullMode;
	bool mCullModeDirty;

	FillMode mFillMode;
	bool mFillModeDirty;

	PRIMITIVE_TOPOLOGY mPrimitiveTopology;
	bool mPrimitiveTopologyDirty;

	ViewPort mViewport;
	bool mViewportDirty;

	std::vector<BindingSamplerState> mSamplerState;
	bool mSamplerStateDirty;
};

}