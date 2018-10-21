#include "stateManager.h"

namespace Cjing3D {

StateManager::StateManager(GraphicsDevice& device) :
		mDevice(device)
{
	for (int i = 0; i < DepthStencilStateID::Count; i++){
		mDepthStencilStates[static_cast<U32>(i)] = std::make_unique<DepthStencilState>();
	}
}

StateManager::~StateManager()
{
	for (int i = 0; i < DepthStencilStateID::Count; i++)
	{
		mDepthStencilStates[static_cast<U32>(i)].reset();
	}
}

void StateManager::SetupStates()
{
	SetupDepthStencilStates();
	SetupBlendStates();
}

DepthStencilState & StateManager::GetDepthStencilState(DepthStencilStateID id)
{
	Debug::CheckAssertion(id < DepthStencilStateID::Count);
	return *mDepthStencilStates[static_cast<U32>(id)];
}

void StateManager::SetupDepthStencilStates()
{
	{
		DepthStencilStateDesc desc;
		desc.mDepthEnable = false;
		desc.mStencilEnable = false;
		const auto result = mDevice.CreateDepthStencilState(desc, *mDepthStencilStates[DepthStencilStateID::DepthNone]);
		Debug::ThrowIfFailed(result, "Failed to create none depthStencilState", result);
	}
	{
		DepthStencilStateDesc desc;
		desc.mDepthEnable = true;
		desc.mDepthWriteMask = DEPTH_WRITE_MASK_ALL;
		desc.mDepthFunc = COMPARISON_GREATER_EQUAL;

		desc.mStencilEnable = true;
		desc.mStencilReadMask = 0xFF;
		desc.mStencilWriteMask = 0xFF;
		desc.mFrontFace.mDepthFunc = COMPARISON_ALWAYS;
		desc.mFrontFace.mStencilPassOp = STENCIL_OP_REPLACE;
		desc.mFrontFace.mStencilFailOp = STENCIL_OP_KEEP;
		desc.mFrontFace.mStencilDepthFailOp = STENCIL_OP_KEEP;
		desc.mBackFace.mDepthFunc = COMPARISON_ALWAYS;
		desc.mBackFace.mStencilPassOp = STENCIL_OP_REPLACE;
		desc.mBackFace.mStencilFailOp = STENCIL_OP_KEEP;
		desc.mBackFace.mStencilDepthFailOp = STENCIL_OP_KEEP;

		const auto result = mDevice.CreateDepthStencilState(desc, *mDepthStencilStates[DepthStencilStateID::GreaterEqualReadWrite]);
		Debug::ThrowIfFailed(result, "Failed to create greater equal depthStencilState", result);
	}

}

void StateManager::SetupBlendStates()
{
}

}
