#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\renderer.h"
#include "renderer\renderResource.h"

namespace Cjing3D
{
	// depthStencilState
	enum DepthStencilStateID {
		DepthStencilStateID_DepthNone = 0,
		DepthStencilStateID_GreaterEqualReadWrite,
		DepthStencilStateID_Count
	};

	// blend State
	enum BlendStateID {
		BlendStateID_Opaque = 0,
		BlendStateID_Transpranent,
		BlendStateID_Count
	};

	// Rasterizer State
	enum RasterizerStateID {
		RasterizerStateID_Front = 0,
		RasterizerStateID_Count
	};

	class StateManager
	{
	public:
		StateManager(GraphicsDevice& device);
		~StateManager();

		void SetupStates();

		DepthStencilState& GetDepthStencilState(DepthStencilStateID id);

	private:
		void SetupDepthStencilStates();
		void SetupBlendStates();
		void SetupRasterizerStates();

	private:
		GraphicsDevice & mDevice;

		std::unique_ptr<DepthStencilState> mDepthStencilStates[DepthStencilStateID::DepthStencilStateID_Count];
		std::unique_ptr<BlendState> mBlendStates[BlendStateID::BlendStateID_Count];
		std::unique_ptr<RasterizerState> mRasterizerStates[RasterizerStateID::RasterizerStateID_Count];
	};
}