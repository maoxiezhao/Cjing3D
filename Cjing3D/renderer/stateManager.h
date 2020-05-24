#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\renderer.h"
#include "renderer\RHI\rhiResource.h"

namespace Cjing3D
{
	// depthStencilState
	enum DepthStencilStateID {
		DepthStencilStateID_DepthNone = 0,
		DepthStencilStateID_GreaterEqualReadWrite,
		DepthStencilStateID_DepthRead,
		DepthStencilStateID_DepthReadEqual,
		DepthStencilStateID_Shadow,
		DepthStencilStateID_Count
	};

	// blend State
	enum BlendStateID {
		BlendStateID_Opaque = 0,
		BlendStateID_Transpranent,
		BlendStateID_PreMultiplied,
		BlendStateID_ColorWriteDisable,
		BlendStateID_Count
	};

	// Rasterizer State
	enum RasterizerStateID {
		RasterizerStateID_Front = 0,
		RasterizerStateID_Image,
		RasterizerStateID_Shadow,
		RasterizerStateID_WireFrame,
		RasterizerStateID_Sky,
		RasterizerStateID_Count
	};

	// Sampler State
	enum SamplerStateID {
		SamplerStateID_PointClampAlways,
		SamplerStateID_PointClampGreater,
		SamplerStateID_LinearClampGreater,
		SamplerStateID_LinearWrapGreater,
		SamplerStateID_ANISOTROPIC,
		SamplerStateID_Comparision_depth,
		SamplerStateID_Count
	};

	class StateManager
	{
	public:
		StateManager(GraphicsDevice& device);
		~StateManager();

		void Initialize();
		void Uninitalize();

		std::shared_ptr<DepthStencilState> GetDepthStencilState(DepthStencilStateID id);
		std::shared_ptr<BlendState> GetBlendState(BlendStateID id);
		std::shared_ptr<RasterizerState> GetRasterizerState(RasterizerStateID id);
		std::shared_ptr<SamplerState> GetSamplerState(SamplerStateID id);

		std::vector<std::shared_ptr<SamplerState> > GetCommonSampleStates();

	private:
		void SetupDepthStencilStates();
		void SetupBlendStates();
		void SetupRasterizerStates();
		void SetupSamplerStates();

	private:
		GraphicsDevice & mDevice;

		std::shared_ptr<DepthStencilState> mDepthStencilStates[DepthStencilStateID_Count];
		std::shared_ptr<BlendState> mBlendStates[BlendStateID_Count];
		std::shared_ptr<RasterizerState> mRasterizerStates[RasterizerStateID_Count];
		std::shared_ptr<SamplerState> mSamplerStates[SamplerStateID_Count];

	};
}