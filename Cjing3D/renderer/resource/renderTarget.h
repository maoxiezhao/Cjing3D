#pragma once

#include "renderer\RHI\rhiTexture.h"

namespace Cjing3D
{

class Renderer;

class DepthTarget
{
public:
	DepthTarget(Renderer& renderer);
	~DepthTarget();

	void Setup(U32 width, U32 height);
	void Clear();
	RhiTexture2DPtr GetTexture();

private:
	RhiTexture2DPtr mTexture;

	Renderer& mRenderer;
};

class RenderTarget
{
public:
	RenderTarget(Renderer& renderer);
	~RenderTarget();

	void Setup(U32 width, U32 height, FORMAT format, U32 mipMapLevelCount, bool hasDepth = false);
	void Clear();
	void Bind(F32 r, F32 g, F32 b, F32 a);
	void UnBind();

private:
	U32 mNumViews;
	std::vector<RhiTexture2DPtr> mRenderTargets;
	ViewPort mViewPort;
	std::shared_ptr<DepthTarget> mDepthTarget;

	Renderer& mRenderer;
};

}