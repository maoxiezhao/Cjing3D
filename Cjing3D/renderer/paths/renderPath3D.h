#pragma once

#include "renderPath2D.h"

namespace Cjing3D {

	class RenderPath3D : public RenderPath2D
	{
	public:
		static const FORMAT RenderTargetFormatHDR = FORMAT_R16G16B16A16_FLOAT;
		static const FORMAT DepthStencilFormat = FORMAT_D32_FLOAT_S8X24_UINT;
		static const FORMAT DepthStencilFormatAlias = FORMAT_R32G8X24_TYPELESS;

		RenderPath3D();
		~RenderPath3D();

		virtual void Update(F32 dt);
		virtual void ResizeBuffers();
		virtual void Compose();

		// render method
		virtual void RenderShadowmaps();
		virtual void RenderTransparents(RenderBehavior& renderBehavior, RenderPassType renderType);
		virtual void RenderPostprocess(Texture2D& rtScreen);

		void SetExposure(F32 exposure) { mExposure = exposure; }
		F32 GetExposure()const { return mExposure; }

		bool IsFXAAEnable()const { return mEnableFXAA; }
		void SetFXAAEnable(bool enableFXAA) { mEnableFXAA = enableFXAA; }
		bool IsShadowEnable()const { return mEnableShadow; }
		void SetShadowEnable(bool enableShadow) { mEnableShadow = enableShadow; }
		U32 GetMSAASampleCount()const { return mMSAASampleCount; }
	
	protected:
		Texture2D* GetLastPostprocessRT() {
			return &mRTPostprocessLDR2;
		}
		Texture2D* GetDepthBuffer() {
			return &mDepthBuffer;
		}
		Texture2D* GetDepthBufferTemp() {
			return &mDepthBufferTemp;
		}

		Texture2D mDepthBuffer;
		Texture2D mDepthBufferTemp;
		Texture2D mRTPostprocessLDR1;
		Texture2D mRTPostprocessLDR2;

		// tone mapping ÆØ¹âÏµÊý
		F32 mExposure = 1.0f;				
		bool mEnableFXAA = false;
		bool mEnableShadow = true;
		U32 mMSAASampleCount = 0;
	};

}