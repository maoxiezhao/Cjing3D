#pragma once

#include "renderer\RHI\rhiStructure.h"
#include "renderer\includeD3D11.h"

namespace Cjing3D
{
	class GraphicsDeviceChild
	{
	public:
		std::shared_ptr<void> mRhiState = nullptr;
		inline bool IsValid()const { return mRhiState.get() != nullptr; }
		inline void Clear() { mRhiState = nullptr; }
	};

	//***********************************************************************
	// GPU Resources
	//***********************************************************************

	enum GPU_RESOURCE_TYPE
	{
		BUFFER,
		TEXTURE_1D,
		TEXTURE_2D,
		UNKNOWN_TYPE
	};

	class GPUResource : public GraphicsDeviceChild
	{
	public:
		inline bool IsBuffer()const { return mCurrType == GPU_RESOURCE_TYPE::BUFFER; }
		inline bool IsTexture()const { return mCurrType == GPU_RESOURCE_TYPE::TEXTURE_1D || mCurrType == GPU_RESOURCE_TYPE::TEXTURE_2D; }

		GPU_RESOURCE_TYPE mCurrType = GPU_RESOURCE_TYPE::UNKNOWN_TYPE;
	};

	class GPUBuffer : public GPUResource
	{
	public:
		GPUBufferDesc mDesc;
		GPUBufferDesc GetDesc() { return mDesc; }
		void SetDesc(GPUBufferDesc desc) {	mDesc = desc;}
	};

	class RhiTexture : public GPUResource
	{
	public:
		TextureDesc mDesc;
		const TextureDesc& GetDesc()const { return mDesc; }
		void SetDesc(TextureDesc& desc) { mDesc = desc; }
	};

	class RhiTexture2D : public RhiTexture {};

	using Texture      = RhiTexture;
	using Texture2D    = RhiTexture2D;
	using TexturePtr   = std::shared_ptr<Texture>;
	using Texture2DPtr = std::shared_ptr<Texture2D>;

	//***********************************************************************
	// GPU States
	//***********************************************************************

	class RasterizerState : public GraphicsDeviceChild
	{
	public:
		RasterizerStateDesc mDesc;
		void SetDesc(RasterizerStateDesc desc) { mDesc = desc; }
		RasterizerStateDesc GetDesc()const { return mDesc; }
	};

	class DepthStencilState : public GraphicsDeviceChild
	{
	public:
		DepthStencilStateDesc mDesc;
		void SetDesc(DepthStencilStateDesc desc) { mDesc = desc; }
		DepthStencilStateDesc GetDesc()const { return mDesc; }
	};

	class BlendState : public GraphicsDeviceChild
	{
	public:
		BlendStateDesc mDesc;
		void SetDesc(BlendStateDesc desc) { mDesc = desc; }
		BlendStateDesc GetDesc()const { return mDesc; }
	};

	class SamplerState : public GraphicsDeviceChild
	{
	public:
		SamplerDesc mDesc;
		void SetDesc(const SamplerDesc& desc) { mDesc = desc; }
		SamplerDesc GetDesc()const { return mDesc; }
	};

	class InputLayout : public GraphicsDeviceChild
	{
	public:
		std::vector<VertexLayoutDesc> mDescs;
		const std::vector<VertexLayoutDesc>& GetDesc()const { return mDescs; }
	};
	using InputLayoutPtr = std::shared_ptr<InputLayout>;


	struct RenderBehaviorParam
	{
		enum RenderType
		{
			RenderType_RenderTarget,
			RenderType_DepthStencil
		};
		RenderType mType = RenderType_RenderTarget;
		const Texture* mTexture = nullptr;
		I32 mSubresourceIndex = -1;

		enum RenderOperation
		{
			RenderOperation_Load,
			RenderOperation_Clear
		};
		RenderOperation mOperation = RenderOperation_Load;
	};

	struct RenderBehaviorDesc
	{
		std::vector<RenderBehaviorParam> mParams;
	};

	class RenderBehavior : public GraphicsDeviceChild
	{
	public:
		RenderBehaviorDesc mDesc;
	};

	class GPUQuery : public GraphicsDeviceChild
	{
	public:
		GPUQueryDesc mDesc;
	};
}