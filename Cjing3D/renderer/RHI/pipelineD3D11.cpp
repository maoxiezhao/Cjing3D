#include "pipelineD3D11.h"

namespace Cjing3D {
	namespace PipelineD3D11 {

		/**---------------------------------------
		*	Input Assembler State
		*----------------------------------------*/

		void IA::BindPrimitiveTopology(
			ID3D11DeviceContext & context,
			D3D11_PRIMITIVE_TOPOLOGY topology)
		{
			// ����ͼԪ����
			context.IASetPrimitiveTopology(topology);
		}

		void IA::BindInputLayout(
			ID3D11DeviceContext & deviceContext,
			ID3D11InputLayout & inputLayout)
		{
			// ����InputLayout
			deviceContext.IASetInputLayout(&inputLayout);
		}

		void IA::BindVertexBuffer(ID3D11DeviceContext & context, ID3D11Buffer& buffer, U32 slot, U32 stride, U32 offset)
		{
			// �󶨶��㻺��
			ID3D11Buffer* const buffers[] = { &buffer };
			BindVertexBuffers(context, 1, buffers, slot, &stride, &offset);
		}

		void IA::BindVertexBuffers(ID3D11DeviceContext & context, U32 numBuffers, ID3D11Buffer * const * buffers, U32 slot, const U32 * stride, const U32* offset)
		{
			// ��һЩ�ж��㻺��
			context.IASetVertexBuffers(slot, numBuffers, buffers, stride, offset);
		}

		void IA::BindIndexBuffer(ID3D11DeviceContext & context, ID3D11Buffer & buffer, DXGI_FORMAT format, U32 offset)
		{
			// ����������
			context.IASetIndexBuffer(&buffer, format, offset);
		}

		/**---------------------------------------
		*	Vertex Shader Stage
		*----------------------------------------*/

		void VS::BindShader(
			ID3D11DeviceContext & context,
			ID3D11VertexShader * shader)
		{
			// �󶨶�����ɫ��
			context.VSSetShader(shader, nullptr, 0);
		}

		void VS::BindConstantBuffer(ID3D11DeviceContext & context, U32 slot, ID3D11Buffer * buffer)
		{
			// �󶨶���׶ε�ConstantBuffer
			ID3D11Buffer* const buffers[] = { buffer };
			context.VSSetConstantBuffers(slot, 1, buffers);
		}

		/**---------------------------------------
		*	Raterizer Stage
		*----------------------------------------*/

		void RS::SetState(
			ID3D11DeviceContext & context,
			ID3D11RasterizerState * state)
		{
			// ���ù�դ��״̬
			context.RSSetState(state);
		}

		void RS::BindViewport(ID3D11DeviceContext & context, U32 numViewports ,const D3D11_VIEWPORT* viewport)
		{
			// ����viewport
			context.RSSetViewports(1, viewport);
		}

		/**---------------------------------------
		*	Pixel Stage
		*----------------------------------------*/

		void PS::BindShader(
			ID3D11DeviceContext & context,
			ID3D11PixelShader * shader)
		{
			context.PSSetShader(shader, nullptr, 0);
		}

		void PS::BindShaderResourceView(
			ID3D11DeviceContext & context,
			uint32_t slot,
			ID3D11ShaderResourceView* srv)
		{
			// ����ShaderResourceView
			context.PSSetShaderResources(slot, 1, &srv);
		}

		void PS::BindShaderResourceViews(
			ID3D11DeviceContext & context,
			uint32_t slot,
			uint32_t nums,
			ID3D11ShaderResourceView * const * srv)
		{
			// ����ShaderResourceViews
			context.PSSetShaderResources(slot, nums, srv);
		}

		void PS::BindConstantBuffer(ID3D11DeviceContext & context, U32 slot, ID3D11Buffer * buffer)
		{
			// �󶨶���׶ε�ConstantBuffer
			ID3D11Buffer* const buffers[] = { buffer };
			context.PSSetConstantBuffers(slot, 1, buffers);
		}

		/**---------------------------------------
		*	Output Merge Stage
		*----------------------------------------*/

		void OM::BindRTVAndDSV(ID3D11DeviceContext & context, ID3D11RenderTargetView * rtv, ID3D11DepthStencilView * dsv)
		{
			// ������ȾĿ����ͼ�����ģ����ͼ
			context.OMSetRenderTargets(1, &rtv, dsv);
		}

		void OM::ClearRenderTargetView(
			ID3D11DeviceContext & context,
			ID3D11RenderTargetView * rtv)
		{
			static const float rgba[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
			context.ClearRenderTargetView(rtv, rgba);
		}

		void OM::ClearDepthOfDSV(ID3D11DeviceContext & context, ID3D11DepthStencilView * dsv, F32 depth)
		{
			// ������ģ����ͼ�е��������
			context.ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH, depth, 0);
		}

		void OM::BindBlendState(ID3D11DeviceContext & context, ID3D11BlendState * state, U32 sampleMask)
		{
			// ���û��״̬
			context.OMSetBlendState(state, nullptr, sampleMask);
		}

		void OM::BindDepthStencilState(ID3D11DeviceContext & context, ID3D11DepthStencilState * state)
		{
			// �����ģ��״̬
			context.OMSetDepthStencilState(state, 0);
		}

	}
}