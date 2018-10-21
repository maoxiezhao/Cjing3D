#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\renderer.h"
#include "renderer\renderResource.h"

namespace Cjing3D
{
	// depthStencilState
	enum DepthStencilStateID {
		DepthNone = 0,
		GreaterEqualReadWrite,
		Count
	};

	// blend State
	enum BlendStateID {
		Opaque = 0,
		Transpranent,
		Count
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

	private:
		GraphicsDevice & mDevice;

		std::unique_ptr<DepthStencilState> mDepthStencilStates[DepthStencilStateID::Count];
		std::unique_ptr<> mBlendStates[BlendStateID::Count];
	};
}