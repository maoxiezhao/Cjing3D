#pragma once

#include "renderPath2D.h"

namespace Cjing3D {

	class RenderPath3D : public RenderPath2D
	{
	public:
		//////////////////////////////////////////////////////////////////////////////////////
		
		static const FORMAT RenderTargetFormatHDR = FORMAT_R16G16B16A16_FLOAT;
		static const FORMAT DepthStencilFormat = FORMAT_D32_FLOAT_S8X24_UINT;
		static const FORMAT DepthStencilFormatAlias = FORMAT_R32G8X24_TYPELESS;

		enum AOTYPE
		{
			AOTYPE_DISABLE,
			AOTYPE_SSAO,
		};

		//////////////////////////////////////////////////////////////////////////////////////

		RenderPath3D();
		~RenderPath3D();

		virtual void Update(F32 dt);
		virtual void ResizeBuffers();
		virtual void Compose(CommandList cmd);

		// render method
		virtual void RenderShadowmaps(CommandList cmd);
		virtual void RenderDepthLinear(CommandList cmd);
		virtual void RenderAO(CommandList cmd);
		virtual void RenderTransparents(CommandList cmd, RenderBehavior& renderBehavior, RenderPassType renderType);
		virtual void RenderPostprocess(CommandList cmd, Texture2D& rtScreen);
		void RenderDebug(CommandList cmd);

		void SetExposure(F32 exposure) { mExposure = exposure; }
		F32 GetExposure()const { return mExposure; }
		bool IsFXAAEnable()const { return mEnableFXAA; }
		void SetFXAAEnable(bool enableFXAA) { mEnableFXAA = enableFXAA; }
		bool IsShadowEnable()const { return mEnableShadow; }
		void SetShadowEnable(bool enableShadow) { mEnableShadow = enableShadow; }
		U32 GetMSAASampleCount()const { return mMSAASampleCount; }
		void SetAORange(F32 range) { mAORange = range; }
		F32 GetAORange()const { return mAORange; }
		void SetAOSampleCount(U32 count) { mAOSampleCount = count; }
		U32 GetAOSampleCount()const { return mAOSampleCount; }
		bool IsAOEnable()const { return mAOType != AOTYPE_DISABLE; }
		void SetAOType(AOTYPE type) { mAOType = type; }
		AOTYPE GetAOType()const { return mAOType; }
		void SetRenderLayerMask(U32 layerMask) { mRenderLayerMask = layerMask; }
		U32 GetRenderLayerMask()const { return mRenderLayerMask; }

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

		Texture2D* GetDepthBufferLinear() {
			return &mDepthBufferLinear;
		}
		Texture2D* GetAOTexture() {
			return &mAOTexture;
		}

	protected:
		Texture2D mDepthBuffer;
		Texture2D mDepthBufferTemp;
		Texture2D mDepthBufferLinear;
		Texture2D mRTPostprocessLDR1;
		Texture2D mRTPostprocessLDR2;

		// general params
		U32 mRenderLayerMask = 0xFFFFFFFF;

		// tone mapping ÆØ¹âÏµÊý
		F32 mExposure = 1.0f;				
		bool mEnableFXAA = false;
		bool mEnableShadow = true;
		U32 mMSAASampleCount = 0;

		// AO
		AOTYPE mAOType = AOTYPE_SSAO;
		F32 mAORange = 1.0f;
		U32 mAOSampleCount = 16;
		Texture2D mAOTexture;
	};

}