#include "stateManager.h"

namespace Cjing3D {

StateManager::StateManager(GraphicsDevice& device) :
		mDevice(device)
{
	for (int i = 0; i < DepthStencilStateID::DepthStencilStateID_Count; i++){
		mDepthStencilStates[static_cast<U32>(i)] = std::make_unique<DepthStencilState>();
	}

	for (int i = 0; i < BlendStateID::BlendStateID_Count; i++) {
		mBlendStates[static_cast<U32>(i)] = std::make_unique<BlendState>();
	}

	for (int i = 0; i < RasterizerStateID::RasterizerStateID_Count; i++) {
		mRasterizerStates[static_cast<U32>(i)] = std::make_unique<RasterizerState>();
	}
}

StateManager::~StateManager()
{
	for (int i = 0; i < DepthStencilStateID::DepthStencilStateID_Count; i++){
		mDepthStencilStates[static_cast<U32>(i)].reset();
	}
}

void StateManager::SetupStates()
{
	SetupDepthStencilStates();
	SetupBlendStates();
	SetupRasterizerStates();
}

DepthStencilState & StateManager::GetDepthStencilState(DepthStencilStateID id)
{
	Debug::CheckAssertion(id < DepthStencilStateID::DepthStencilStateID_Count);
	return *mDepthStencilStates[static_cast<U32>(id)];
}

void StateManager::SetupDepthStencilStates()
{
	{
		DepthStencilStateDesc desc;
		desc.mDepthEnable = false;
		desc.mStencilEnable = false;
		const auto result = mDevice.CreateDepthStencilState(desc, *mDepthStencilStates[DepthStencilStateID::DepthStencilStateID_DepthNone]);
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

		const auto result = mDevice.CreateDepthStencilState(desc, *mDepthStencilStates[DepthStencilStateID::DepthStencilStateID_GreaterEqualReadWrite]);
		Debug::ThrowIfFailed(result, "Failed to create greater equal depthStencilState", result);
	}
}

void StateManager::SetupBlendStates()
{
	{
		BlendStateDesc desc = {};
		desc.mAlphaToCoverageEnable = false;
		desc.mIndependentBlendEnable = false;
		desc.mRenderTarget[0].mBlendEnable = false;
		desc.mRenderTarget[0].mSrcBlend = BLEND_SRC_ALPHA;
		desc.mRenderTarget[0].mDstBlend = BLEND_INV_SRC_ALPHA;
		desc.mRenderTarget[0].mBlendOp = BLEND_OP_MAX;
		desc.mRenderTarget[0].mSrcBlendAlpha = BLEND_ONE;
		desc.mRenderTarget[0].mDstBlendAlpha = BLEND_ZERO;
		desc.mRenderTarget[0].mBlendOpAlpha = BLEND_OP_ADD;
		desc.mRenderTarget[0].mRenderTargetWriteMask = COLOR_WRITE_ENABLE_ALL;
		const auto result = mDevice.CreateBlendState(desc, *mBlendStates[BlendStateID::BlendStateID_Opaque]);
		Debug::ThrowIfFailed(result, "Failed to create opaque blendState", result);
	}
	{
		BlendStateDesc desc = {};
		desc.mAlphaToCoverageEnable = false;
		desc.mIndependentBlendEnable = false;
		desc.mRenderTarget[0].mBlendEnable = true;
		desc.mRenderTarget[0].mSrcBlend = BLEND_SRC_ALPHA;
		desc.mRenderTarget[0].mDstBlend = BLEND_INV_SRC_ALPHA;
		desc.mRenderTarget[0].mBlendOp = BLEND_OP_ADD;
		desc.mRenderTarget[0].mSrcBlendAlpha = BLEND_ONE;
		desc.mRenderTarget[0].mDstBlendAlpha = BLEND_ONE;
		desc.mRenderTarget[0].mBlendOpAlpha = BLEND_OP_ADD;
		desc.mRenderTarget[0].mRenderTargetWriteMask = COLOR_WRITE_ENABLE_ALL;
		const auto result = mDevice.CreateBlendState(desc, *mBlendStates[BlendStateID::BlendStateID_Transpranent]);
		Debug::ThrowIfFailed(result, "Failed to create transparent blendState", result);
	}
}

void StateManager::SetupRasterizerStates()
{
}

}
