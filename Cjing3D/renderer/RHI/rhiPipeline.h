#pragma once

#include "renderer\renderableCommon.h"

// need to refactor
#include "renderer\RHI\rhiResource.h"
#include "renderer\RHI\rhiShader.h"
#include "renderer\RHI\rhiBuffer.h"
#include "renderer\RHI\rhiTexture.h"

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

using BindingConstantBuffer = BindingGraphicsResource<GPUBuffer>;
using BindingSamplerState = BindingGraphicsResource<SamplerState>;
using BindingTexture = BindingGraphicsResource<RhiTexture2D>;

class GraphicsDevice;

class Pipeline
{
public:
	Pipeline(GraphicsDevice& device);
	~Pipeline();

	void Bind();
	void Clear();

	void SetViewport(ViewPort viewport);

	void SetVertexShader(VertexShaderInfo shader);
	void SetPixelShader(PixelShaderPtr shader);

	void SetInputLayout(std::shared_ptr<InputLayout> layout);

	void SetTexture(RhiTexture2DPtr texture, U32 slot, SHADERSTAGES stage);

	void SetRenderTargets(const std::vector<RhiTexture2DPtr>& textures, RhiTexture2D* depthStencilTexture = nullptr);

	void SetSampler(std::shared_ptr<SamplerState> sampler, U32 slot, SHADERSTAGES stage);

	void SetConstantBuffer(std::shared_ptr<GPUBuffer> buffer, U32 slot, SHADERSTAGES stage);

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

	ShaderInfoState mShaderInfoState;
	bool mShaderInfoStateDirty;

	std::vector<BindingSamplerState> mSamplerState;
	bool mSamplerStateDirty;

	std::vector<BindingTexture> mTextures;
	bool mTextureDirty = false;

	std::vector<RhiTexture2DPtr> mRenderTargetViews;
	RhiTexture2D* mDepthStencilTexture;
	bool mRenderTargetViewDirty = false;
};

}