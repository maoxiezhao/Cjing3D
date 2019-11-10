#include "stateManager.h"
#include "renderer\RHI\device.h"
#include "renderer\RHI\rhiFactory.h"

namespace Cjing3D {

StateManager::StateManager(GraphicsDevice& device) :
		mDevice(device)
{
	for (int i = 0; i < DepthStencilStateID::DepthStencilStateID_Count; i++){
		mDepthStencilStates[static_cast<U32>(i)] = std::make_shared<DepthStencilState>();
	}

	for (int i = 0; i < BlendStateID::BlendStateID_Count; i++) {
		mBlendStates[static_cast<U32>(i)] = std::make_shared<BlendState>();
	}

	for (int i = 0; i < RasterizerStateID::RasterizerStateID_Count; i++) {
		mRasterizerStates[static_cast<U32>(i)] = std::make_shared<RasterizerState>();
	}

	for (int i = 0; i < SamplerStateID_Count; i++) {
		mSamplerStates[static_cast<U32>(i)] = std::make_shared<SamplerState>();
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
	SetupSamplerStates();
}

std::shared_ptr<DepthStencilState> StateManager::GetDepthStencilState(DepthStencilStateID id)
{
	Debug::CheckAssertion(id < DepthStencilStateID::DepthStencilStateID_Count);
	return mDepthStencilStates[static_cast<U32>(id)];
}

std::shared_ptr<BlendState> StateManager::GetBlendState(BlendStateID id)
{
	Debug::CheckAssertion(id < BlendStateID::BlendStateID_Count);
	return mBlendStates[static_cast<U32>(id)];
}

std::shared_ptr<RasterizerState> StateManager::GetRasterizerState(RasterizerStateID id)
{
	Debug::CheckAssertion(id < RasterizerStateID::RasterizerStateID_Count);
	return mRasterizerStates[static_cast<U32>(id)];
}

std::shared_ptr<SamplerState> StateManager::GetSamplerState(SamplerStateID id)
{
	Debug::CheckAssertion(id < SamplerStateID::SamplerStateID_Count);
	return mSamplerStates[static_cast<U32>(id)];
}

std::vector<std::shared_ptr<SamplerState>> StateManager::GetCommonSampleStates()
{
	return std::vector<std::shared_ptr<SamplerState>>();
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

	DepthStencilStateDesc desc;
	desc.mDepthEnable = true;
	desc.mDepthWriteMask = DEPTH_WRITE_MASK_ALL;
	desc.mDepthFunc = COMPARISON_GREATER_EQUAL;
	desc.mStencilEnable = false;
	desc.mStencilReadMask = 0xFF;
	desc.mStencilWriteMask = 0xFF;

	desc.mFrontFace.mStencilFunc = COMPARISON_GREATER_EQUAL;
	desc.mFrontFace.mStencilPassOp = STENCIL_OP_KEEP;
	desc.mFrontFace.mStencilFailOp = STENCIL_OP_KEEP;
	desc.mFrontFace.mStencilDepthFailOp = STENCIL_OP_KEEP;

	desc.mBackFace.mStencilFunc = COMPARISON_GREATER_EQUAL;
	desc.mBackFace.mStencilPassOp = STENCIL_OP_KEEP;
	desc.mBackFace.mStencilFailOp = STENCIL_OP_KEEP;
	desc.mBackFace.mStencilDepthFailOp = STENCIL_OP_KEEP;

	{
		desc.mDepthEnable = true;
		desc.mDepthWriteMask = DEPTH_WRITE_MASK_ALL;
		desc.mDepthFunc = COMPARISON_GREATER_EQUAL;

		const auto result = mDevice.CreateDepthStencilState(desc, *mDepthStencilStates[DepthStencilStateID::DepthStencilStateID_GreaterEqualReadWrite]);
		Debug::ThrowIfFailed(result, "Failed to create greater equal depthStencilState", result);
	}

	{
		desc.mDepthEnable = true;
		desc.mDepthWriteMask = DEPTH_WRITE_MASK_ZERO;
		desc.mDepthFunc = COMPARISON_EQUAL;

		const auto result = mDevice.CreateDepthStencilState(desc, *mDepthStencilStates[DepthStencilStateID::DepthStencilStateID_DepthReadEqual]);
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
		desc.mRenderTarget[0].mRenderTargetWriteMask = COLOR_WRITE_ENABLE_ALL;
		desc.mRenderTarget[0].mSrcBlend = BLEND_SRC_ALPHA;
		desc.mRenderTarget[0].mDstBlend = BLEND_INV_SRC_ALPHA;
		desc.mRenderTarget[0].mBlendOp = BLEND_OP_MAX;
		desc.mRenderTarget[0].mSrcBlendAlpha = BLEND_ONE;
		desc.mRenderTarget[0].mDstBlendAlpha = BLEND_ZERO;
		desc.mRenderTarget[0].mBlendOpAlpha = BLEND_OP_ADD;
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
	{
		RasterizerStateDesc desc = {};
		desc.mFillMode = FILL_SOLID;
		desc.mCullMode = CULL_BACK;
		desc.mFrontCounterClockwise = true;
		desc.mDepthBias = 0;
		desc.mDepthBiasClamp = 0;
		desc.mSlopeScaleDepthBias = 0;
		desc.mDepthClipEnable = true;
		desc.mMultisampleEnable = false;
		desc.mAntialiaseLineEnable = false;
		desc.mConservativeRasterizationEnable = false;

		const auto result = mDevice.CreateRasterizerState(desc, *mRasterizerStates[RasterizerStateID::RasterizerStateID_Front]);
		Debug::ThrowIfFailed(result, "Failed to create opaque blendState", result);
	}
}

void StateManager::SetupSamplerStates()
{
	{
		const auto result = CreateDefaultSamplerState(mDevice, *mSamplerStates[SamplerStateID_PointClampAlways],
			FILTER_MIN_MAG_MIP_POINT, TEXTURE_ADDRESS_CLAMP, COMPARISON_ALWAYS);
		Debug::ThrowIfFailed(result, "Failed to create pointClampAlways samplerState", result);
	}

	{
		const auto result = CreateDefaultSamplerState(mDevice, *mSamplerStates[SamplerStateID_PointClampGreater],
			FILTER_MIN_MAG_MIP_POINT, TEXTURE_ADDRESS_CLAMP, COMPARISON_GREATER_EQUAL);
		Debug::ThrowIfFailed(result, "Failed to create pointClampAlways samplerState", result);
	}

	{
		const auto result = CreateDefaultSamplerState(mDevice, *mSamplerStates[SamplerStateID_LinearClampGreater],
			FILTER_MIN_MAG_LINEAR_MIP_POINT, TEXTURE_ADDRESS_CLAMP, COMPARISON_GREATER_EQUAL);
		Debug::ThrowIfFailed(result, "Failed to create pointClampAlways samplerState", result);
	}

	{
		const auto result = CreateDefaultSamplerState(mDevice, *mSamplerStates[SamplerStateID_LinearWrapGreater],
			FILTER_MIN_MAG_LINEAR_MIP_POINT, TEXTURE_ADDRESS_WRAP, COMPARISON_GREATER_EQUAL);
		Debug::ThrowIfFailed(result, "Failed to create pointClampAlways samplerState", result);
	}
}

}
