#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\renderResource.h"
#include "renderer\RHI\rhiDefinition.h"
#include "renderer\RHI\rhiResource.h"
#include "renderer\RHI\rhiStructure.h"

namespace Cjing3D
{
	class GraphicsDevice
	{
	public:
		GraphicsDevice();

		virtual void Initialize();
		virtual void Uninitialize();

		virtual void PresentBegin() = 0;
		virtual void PresentEnd() = 0;

		virtual void BindViewports(const ViewPort* viewports, U32 numViewports, GraphicsThread threadID) = 0;
	
		virtual HRESULT CreateDepthStencilState(const DepthStencilStateDesc& desc, DepthStencilState& state) = 0;
		virtual HRESULT CreateBlendState(const BlendStateDesc& desc, BlendState& state) = 0;

		FORMAT GetBackBufferFormat()const {
			return mBackBufferFormat;
		}

		U32x2 GetScreenSize()const {
			return mScreenSize;
		}

		bool IsMultithreadedRendering()const {
			return mIsMultithreadedRendering;
		}

	protected:
		bool mIsFullScreen;
		FORMAT mBackBufferFormat;
		U32x2 mScreenSize;
		bool mIsMultithreadedRendering;
		bool mIsVsync;						/** �Ƿ�ֱͬ�� */
	};
}