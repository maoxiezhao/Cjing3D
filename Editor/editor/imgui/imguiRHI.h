#pragma once

#include "common\common.h"
#include "core\globalContext.hpp"
#include "renderer\RHI\rhiResource.h"
#include "renderer\RHI\rhiFactory.h"
#include "renderer\RHI\d3d11\deviceD3D11.h"
#include "renderer\RHI\d3d11\swapChainD3D11.h"
#include "renderer\renderer.h"
#include "renderer\preset\renderPreset.h"
#include "utils\math.h"
#include "shaderInterop\shaderInteropImGui.h"

#ifdef _WIN32
#include "platform\win32\gameWindowWin32.h"
#endif

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_internal.h"

using namespace Cjing3D;

namespace ImGui::RHI
{
	namespace {
		GraphicsDeviceD3D11* mDevice = nullptr;
		PipelineState mImGuiPipelineState;
		std::shared_ptr<BlendState> mBlendState = nullptr;
		std::shared_ptr<DepthStencilState> mDepthStencilState = nullptr;
		std::shared_ptr<RasterizerState> mRasterizerState = nullptr;
		GPUBuffer mVertexBuffer;
		GPUBuffer mIndexBuffer;
		GPUBuffer mConstantBuffer;
		Texture2D mFontTextureView;
	    int mVertexBufferSize = 5000, mIndexBufferSize = 10000;

		void CreatFontsTexture()
		{
			ImGuiIO& io = ImGui::GetIO();
			unsigned char* pixels;
			int width, height;
			io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

			TextureDesc desc = {};
			desc.mWidth = width;
			desc.mHeight = height;
			desc.mMipLevels = 1;
			desc.mArraySize = 1;
			desc.mFormat = FORMAT_R8G8B8A8_UNORM;
			desc.mBindFlags = BIND_SHADER_RESOURCE;
			desc.mCPUAccessFlags = 0;

			SubresourceData subresourceData;
			subresourceData.mSysMem = pixels;
			subresourceData.mSysMemPitch = desc.mWidth * 4;

			Renderer::GetDevice().CreateTexture2D(&desc, &subresourceData, mFontTextureView);
		}

		void InitDeviceObject()
		{
			auto& device = Renderer::GetDevice();
			{
				BlendStateDesc desc = {};
				desc.mAlphaToCoverageEnable = false;
				desc.mIndependentBlendEnable = false;
				desc.mRenderTarget[0].mBlendEnable = true;
				desc.mRenderTarget[0].mSrcBlend = BLEND_SRC_ALPHA;
				desc.mRenderTarget[0].mDstBlend = BLEND_INV_SRC_ALPHA;
				desc.mRenderTarget[0].mBlendOp = BLEND_OP_ADD;
				desc.mRenderTarget[0].mSrcBlendAlpha = BLEND_INV_SRC_ALPHA;
				desc.mRenderTarget[0].mDstBlendAlpha = BLEND_ZERO;
				desc.mRenderTarget[0].mBlendOpAlpha = BLEND_OP_ADD;
				desc.mRenderTarget[0].mRenderTargetWriteMask = COLOR_WRITE_ENABLE_ALL;
				const auto result = device.CreateBlendState(desc, *mBlendState);
				Debug::ThrowIfFailed(result, "Failed to create imgui blendState", result);
			}
			{
				DepthStencilStateDesc desc;
				desc.mDepthEnable = false;
				desc.mStencilEnable = false;
				const auto result = device.CreateDepthStencilState(desc, *mDepthStencilState);
				Debug::ThrowIfFailed(result, "Failed to create imgui depthStencilState", result);
			}
			{
				RasterizerStateDesc desc = {};
				desc.mFillMode = FILL_SOLID;
				desc.mCullMode = CULL_NONE;
				desc.mDepthClipEnable = true;

				const auto result = device.CreateRasterizerState(desc, *mRasterizerState);
				Debug::ThrowIfFailed(result, "Failed to create imgui rasterizerState", result);
			}
			{
				GPUBufferDesc desc = {};
				desc.mUsage = USAGE_DEFAULT;
				desc.mCPUAccessFlags = 0;
				desc.mBindFlags = BIND_CONSTANT_BUFFER;
				desc.mByteWidth = sizeof(ImGuiCB);

				const auto result = device.CreateBuffer(&desc, mConstantBuffer, nullptr);
				Debug::ThrowIfFailed(result, "failed to create imgui constant buffer:%08x", result);
			}

			// create pipelineState
			VertexLayoutDesc colorPosLayout[] =
			{
				VertexLayoutDesc::VertexData("POSITION", 0u, FORMAT_R32G32_FLOAT,   0u),
				VertexLayoutDesc::VertexData("TEXCOORD", 0u, FORMAT_R32G32_FLOAT,   0u),
				VertexLayoutDesc::VertexData("COLOR",    0u, FORMAT_R8G8B8A8_UNORM, 0u)
			};
			auto vsInfo = Renderer::LoadVertexShaderInfo("imGuiVS.cso", colorPosLayout, ARRAYSIZE(colorPosLayout));

			PipelineStateDesc desc = {};
			desc.mInputLayout = vsInfo.mInputLayout;
			desc.mVertexShader = vsInfo.mVertexShader;
			desc.mPixelShader = Renderer::LoadShader(SHADERSTAGES_PS, "imGuiPS.cso");
			desc.mPrimitiveTopology  = TRIANGLELIST;
			desc.mBlendState = mBlendState;
			desc.mDepthStencilState = mDepthStencilState;
			desc.mRasterizerState = mRasterizerState;

			Renderer::GetDevice().CreatePipelineState(desc, mImGuiPipelineState);

			// create font texture
			CreatFontsTexture();
		}

		void UninitDeviceObject()
		{
			mConstantBuffer.Clear();
			mFontTextureView.Clear();
			mRasterizerState->Clear();
			mDepthStencilState->Clear();
			mBlendState->Clear();
			mImGuiPipelineState.Clear();
		}

		///////////////////////////////////////////////////////////////

		inline SwapChainD3D11* GetSwapchain(ImGuiViewport* viewport)
		{
			SwapChainD3D11* swapChain = nullptr;
			if (!viewport) {
				return swapChain;
			}

			swapChain = static_cast<SwapChainD3D11*>(viewport->RendererUserData);

			if (!swapChain) {
				return swapChain;
			}

			return swapChain;
		}

		static void ImGuiCreateWindow(ImGuiViewport* viewport)
		{
			if (!viewport) {
				return;
			}
		
			// TEMP
			viewport->RendererUserData = new SwapChainD3D11
			(
				mDevice->GetDevice(),
				mDevice->GetImmediateDeviceContext(),
				(HWND)viewport->PlatformHandle,
				mDevice->IsFullScreen(),
				U32x2(static_cast<U32>(viewport->Size.x), static_cast<U32>(viewport->Size.y)),
				mDevice->ConvertFormat(mDevice->GetBackBufferFormat())
			);
		}

		static void ImGuiDestroyWindow(ImGuiViewport* viewport)
		{
			SwapChainD3D11* swapChain = GetSwapchain(viewport);
			if (!swapChain) {
				return;
			}

			SAFE_DELETE(swapChain);
			viewport->RendererUserData = nullptr;
		}

		static void ImGuiSetWindowSize(ImGuiViewport* viewport, ImVec2 size)
		{
			SwapChainD3D11* swapChain = GetSwapchain(viewport);
			if (!swapChain) {
				return;
			}

			swapChain->Resize(U32x2((U32)size.x, (U32)size.y));
		}

		static void ImGuiRenderWindow(ImGuiViewport* viewport, void*)
		{
			SwapChainD3D11* swapChain = GetSwapchain(viewport);
			if (!swapChain) {
				return;
			}

			// TODO
			CommandList cmd = mDevice->GetCommandList();
			auto& deviceContext = mDevice->GetDeviceContext(cmd);

			ID3D11RenderTargetView* renderTargetView = &swapChain->GetRenderTargetView();
			deviceContext.OMSetRenderTargets(1, &renderTargetView, NULL);
			if (!(viewport->Flags & ImGuiViewportFlags_NoRendererClear)) {
				deviceContext.ClearRenderTargetView(renderTargetView, (float*)&ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
			}

			Render(cmd, viewport->DrawData);

			mDevice->SubmitCommandList();
		}

		static void ImGuiSwapBuffers(ImGuiViewport* viewport, void*)
		{
			SwapChainD3D11* swapChain = GetSwapchain(viewport);
			if (!swapChain) {
				return;
			}

			swapChain->Present(mDevice->GetIsVsync());
		}
	}

	bool Initialize()
	{
		if (!Renderer::IsInitialized())
		{
			Debug::Error("The renderer must is initialized before imgui.");
			return;
		}

		mDevice = dynamic_cast<GraphicsDeviceD3D11*>(&Renderer::GetDevice());

		// Create Device Objects
		InitDeviceObject();

		// init platform
		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;  // We can honor the ImDrawCmd::VtxOffset field, allowing for large meshes.
		io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;  // We can create multi-viewports on the Renderer side (optional)
		io.BackendRendererName = "imgui_impl_cjing3D";

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			InitPlatformInterface();
		}

		return true;
	}

	void Uninitilize()
	{
		UninitDeviceObject();

		mDevice = nullptr;
	}

	void Render(CommandList cmd, ImDrawData* drawData)
	{
		// Avoid rendering when minimized
		if (!drawData || drawData->DisplaySize.x <= 0.0f || drawData->DisplaySize.y <= 0.0f) {
			return;
		}

		auto& device = Renderer::GetDevice();
		if (!mVertexBuffer.IsValid() || mVertexBufferSize < drawData->TotalVtxCount)
		{
			mVertexBufferSize = drawData->TotalVtxCount + 5000;

			GPUBufferDesc desc = {};
			desc.mBindFlags = BIND_VERTEX_BUFFER;
			desc.mByteWidth = static_cast<U32>(mVertexBufferSize * sizeof(ImDrawVert));
			desc.mUsage = USAGE_DYNAMIC;
			desc.mCPUAccessFlags = CPU_ACCESS_WRITE;

			device.CreateBuffer(&desc, mVertexBuffer, nullptr);
		}

		if (!mIndexBuffer.IsValid() || mIndexBufferSize < drawData->TotalIdxCount)
		{
			mIndexBufferSize = drawData->TotalIdxCount + 10000;

			GPUBufferDesc desc = {};
			desc.mBindFlags = BIND_INDEX_BUFFER;
			desc.mByteWidth = static_cast<U32>(mIndexBufferSize * sizeof(ImDrawIdx));
			desc.mUsage = USAGE_DYNAMIC;
			desc.mCPUAccessFlags = CPU_ACCESS_WRITE;

			device.CreateBuffer(&desc, mIndexBuffer, nullptr);
		}

		// Upload vertex/index data into a single contiguous GPU buffer
		GPUResourceMapping vertexMapping, indexMapping;
		vertexMapping.mFlags = indexMapping.mFlags = GPUResourceMapping::FLAG_WRITE;

		device.Map(&mVertexBuffer, vertexMapping);
		device.Map(&mIndexBuffer, indexMapping);
		if (vertexMapping.mData != nullptr && indexMapping.mData != nullptr)
		{
			ImDrawVert* vtxDst = (ImDrawVert*)vertexMapping.mData;
			ImDrawIdx* idxDst = (ImDrawIdx*)indexMapping.mData;

			for (auto i = 0; i < drawData->CmdListsCount; i++)
			{
				const ImDrawList* cmd_list = drawData->CmdLists[i];
				memcpy(vtxDst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
				memcpy(idxDst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
				vtxDst += cmd_list->VtxBuffer.Size;
				idxDst += cmd_list->IdxBuffer.Size;
			}

			device.Unmap(&mVertexBuffer);
			device.Unmap(&mIndexBuffer);
		}

		// Setup orthographic projection matrix into our constant buffer
		// Our visible ImGui space lies from draw_data->DisplayPos (top left) to 
		// draw_data->DisplayPos+data_data->DisplaySize (bottom right). DisplayMin is (0,0) for single viewport apps.
		{
			ImGuiCB cb = {};
			const auto L = drawData->DisplayPos.x;
			const auto R = drawData->DisplayPos.x + drawData->DisplaySize.x;
			const auto T = drawData->DisplayPos.y;
			const auto B = drawData->DisplayPos.y + drawData->DisplaySize.y;
			DirectX::XMStoreFloat4x4(&cb.gImGuiWorldTransform, XMMATRIX
			(
				2.0f / (R - L), 0.0f, 0.0f, (R + L) / (L - R),
				0.0f, 2.0f / (T - B), 0.0f, (T + B) / (B - T),
				0.0f, 0.0f, 0.5f, 0.5f,
				0.0f, 0.0f, 0.0f, 1.0f
			));
			device.UpdateBuffer(cmd, mConstantBuffer, &cb, sizeof(ImGuiCB));
		}

		ViewPort vp;
		vp.mWidth = (F32)drawData->DisplaySize.x;
		vp.mHeight = (F32)drawData->DisplaySize.y;
		device.BindViewports(cmd, &vp, 1);
		device.BindConstantBuffer(cmd, SHADERSTAGES_VS, mConstantBuffer, CB_GETSLOT_NAME(ImGuiCB));
		device.BindPipelineState(cmd, &mImGuiPipelineState);

		auto sampler = Renderer::GetRenderPreset().GetSamplerState(SamplerStateID::SamplerStateID_LinearClamp);
		device.BindSamplerState(cmd, SHADERSTAGES_PS, *sampler, SAMPLER_LINEAR_CLAMP_SLOT);

		GPUBuffer* vbs[] = {
			&mVertexBuffer,
		};
		U32 strides[] = {
			sizeof(ImDrawVert),
		};
		U32 offsets[] = {
			0,
		};
		device.BindVertexBuffer(cmd, vbs, 0, ARRAYSIZE(vbs), strides, offsets);
		device.BindIndexBuffer(cmd, mIndexBuffer, IndexFormat::INDEX_FORMAT_16BIT, 0);

		// Render command lists
		int globalVtxOffset = 0;
		int globalIdxOffset = 0;
		const auto& clip_off = drawData->DisplayPos;
		RectInt scissorRect;
		for (auto i = 0; i < drawData->CmdListsCount; i++)
		{
			auto cmd_list_imgui = drawData->CmdLists[i];
			for (auto cmd_i = 0; cmd_i < cmd_list_imgui->CmdBuffer.Size; cmd_i++)
			{
				const auto pcmd = &cmd_list_imgui->CmdBuffer[cmd_i];
				if (pcmd->UserCallback != nullptr)
				{
					pcmd->UserCallback(cmd_list_imgui, pcmd);
				}
				else
				{
					scissorRect.mLeft   = pcmd->ClipRect.x - clip_off.x;
					scissorRect.mTop    = pcmd->ClipRect.y - clip_off.y;
					scissorRect.mRight  = pcmd->ClipRect.z - clip_off.x;
					scissorRect.mBottom = pcmd->ClipRect.w - clip_off.y;

					device.BindScissorRects(cmd, 1, &scissorRect);
					device.BindGPUResource(cmd, SHADERSTAGES_PS, (Texture2D*)pcmd->TextureId, TEXTURE_SLOT_0);
					device.DrawIndexed(cmd, pcmd->ElemCount, pcmd->IdxOffset + globalIdxOffset, pcmd->VtxOffset + globalVtxOffset);
				}

			}
			globalIdxOffset += cmd_list_imgui->IdxBuffer.Size;
			globalVtxOffset += cmd_list_imgui->VtxBuffer.Size;
		}
	}

	void InitPlatformInterface()
	{
		ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
		platform_io.Renderer_CreateWindow  = ImGuiCreateWindow;
		platform_io.Renderer_DestroyWindow = ImGuiDestroyWindow;
		platform_io.Renderer_SetWindowSize = ImGuiSetWindowSize;
		platform_io.Renderer_RenderWindow  = ImGuiRenderWindow;
		platform_io.Renderer_SwapBuffers   = ImGuiSwapBuffers;
	}
}