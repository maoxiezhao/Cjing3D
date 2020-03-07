#pragma once

#include "renderPath2D.h"

namespace Cjing3D {

	class RenderPath3D : public RenderPath2D
	{
	public:
		static const FORMAT RenderTargetFormatHDR = FORMAT_R16G16B16A16_FLOAT;
		static const FORMAT DepthStencilFormat = FORMAT_D32_FLOAT_S8X24_UINT;
		static const FORMAT DepthStencilFormatAlias = FORMAT_R32G8X24_TYPELESS;

		RenderPath3D(Renderer& renderer);
		~RenderPath3D();

		virtual void Update(F32 dt);
		virtual void ResizeBuffers();
		virtual void Compose();

		// render method
		virtual void RenderTransparents(Texture2D& rtMain, RenderPassType renderType);
		virtual void RenderPostprocess(Texture2D& rtScreen);

		void SetExposure(F32 exposure) { mExposure = exposure; }
		F32 GetExposure()const { return mExposure; }

		bool IsEnableFXAA()const { return mEnableFXAA; }
		void SetEnableFXAA(bool enableFXAA) { mEnableFXAA = enableFXAA; }

	protected:
		Texture2D* GetLastPostprocessRT() {
			return &mRTPostprocessLDR2;
		}

		virtual Texture2D* GetDepthBuffer() {
			return &mDepthBuffer;
		}

		Texture2D mDepthBuffer;
		Texture2D mRTPostprocessLDR1;
		Texture2D mRTPostprocessLDR2;

		// tone mapping �ع�ϵ��
		F32 mExposure = 1.0f;				
		bool mEnableFXAA = false;
	};

}