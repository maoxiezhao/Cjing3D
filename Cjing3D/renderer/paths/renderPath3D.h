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
		virtual void RenderPostprocess(Texture2D& rtScreen);

	protected:
		Texture2D & GetLastPostprocessRT() {
			return mRTPostprocess;
		}

		Texture2D & GetDepthBuffer() {
			return mDepthBuffer;
		}

		Texture2D mDepthBuffer;
		Texture2D mRTPostprocess;
	};

}