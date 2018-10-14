#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\includeD3D11.h"

namespace Cjing3D {
namespace PipelineD3D11 {

	/**---------------------------------------
	*	Input Assembler State
	*----------------------------------------*/
	struct IA {
		static void BindPrimitiveTopology(
			ID3D11DeviceContext& context,
			D3D11_PRIMITIVE_TOPOLOGY topology);

		static void BindInputLayout(
			ID3D11DeviceContext& deviceContext,
			ID3D11InputLayout& inputLayout);

		static void BindVertexBuffer(
			ID3D11DeviceContext& context,
			ID3D11Buffer& buffer,
			U32 slot,
			U32 stride,
			U32 offset = 0);

		static void BindVertexBuffers(
			ID3D11DeviceContext& context,
			U32 numBuffers,
			ID3D11Buffer* const* buffers,
			U32 slot,
			const U32* stride,
			const U32* offset);

		static void BindIndexBuffer(
			ID3D11DeviceContext& context,
			ID3D11Buffer& buffer,
			DXGI_FORMAT format,
			U32 offset = 0);
	};

	/**---------------------------------------
	*	Vertex Shader Stage
	*----------------------------------------*/
	struct VS {
		static void BindShader(
			ID3D11DeviceContext& context,
			ID3D11VertexShader* shader);

		static void BindConstantBuffer(
			ID3D11DeviceContext& context,
			U32 slot,
			ID3D11Buffer* buffer);
	};

	/**---------------------------------------
	*	Raterizer Stage
	*----------------------------------------*/
	struct RS {
		static void SetState(
			ID3D11DeviceContext& context,
			ID3D11RasterizerState* state);

		static void BindViewport(
			ID3D11DeviceContext& context,
			U32 numViewports, 
			const D3D11_VIEWPORT* viewport);
	};

	/**---------------------------------------
	*	Pixel Shader Stage
	*----------------------------------------*/
	struct PS {
		static void BindShader(
			ID3D11DeviceContext& context,
			ID3D11PixelShader* shader);

		static void BindShaderResourceView(
			ID3D11DeviceContext& context,
			uint32_t slot,
			ID3D11ShaderResourceView* srv);

		static void BindShaderResourceViews(
			ID3D11DeviceContext& context,
			uint32_t slot,
			uint32_t nums,
			ID3D11ShaderResourceView* const* srv);

		static void BindConstantBuffer(
			ID3D11DeviceContext& context,
			U32 slot,
			ID3D11Buffer* buffer);
	};

	/**---------------------------------------
	*	Output Merge Stage
	*----------------------------------------*/
	struct OM {
		static void BindRTVAndDSV(
			ID3D11DeviceContext& context,
			ID3D11RenderTargetView* rtv,
			ID3D11DepthStencilView* dsv);

		static void ClearRenderTargetView(
			ID3D11DeviceContext& context,
			ID3D11RenderTargetView* rtv);

		static void ClearDepthOfDSV(
			ID3D11DeviceContext& context,
			ID3D11DepthStencilView* dsv,
			F32 depth = 0.0f);

		static void BindBlendState(
			ID3D11DeviceContext& context,
			ID3D11BlendState* state,
			U32 sampleMask = 0xffffffff);

		static void BindDepthStencilState(
			ID3D11DeviceContext& context,
			ID3D11DepthStencilState* state);
	};

	/**---------------------------------------
	*	Render Method
	*----------------------------------------*/
	static void Draw(
		ID3D11DeviceContext& context,
		int numVertiecs,
		int vertexStart)
	{
		context.Draw(numVertiecs, vertexStart);
	}

	static void DrawIndex(ID3D11DeviceContext& context,
		int numIndices,
		int indexStart,
		int indexOffset = 0)
	{
		context.DrawIndexed(numIndices, indexStart, indexOffset);
	}

	/**---------------------------------------
	*	Resource Method
	*----------------------------------------*/
	static HRESULT Map(
		ID3D11DeviceContext& context,
		ID3D11Resource& resource,
		UINT subResource,
		D3D11_MAP mapType,
		UINT mapFlags,
		D3D11_MAPPED_SUBRESOURCE& mappedResource)
	{
		return context.Map(&resource, subResource, mapType, mapFlags, &mappedResource);
	}

	static void Unmap(
		ID3D11DeviceContext& context,
		ID3D11Resource& resource,
		UINT subResource)
	{
		return context.Unmap(&resource, subResource);
	}


	/**---------------------------------------
	*	GLOBAL METHOD
	*----------------------------------------*/
	struct GENERAL {
		static void BindConstantBuffer(
			ID3D11DeviceContext& context,
			U32 slot,
			ID3D11Buffer* buffer)
		{
			VS::BindConstantBuffer(context, slot, buffer);
			PS::BindConstantBuffer(context, slot, buffer);
		}
	};
}
}