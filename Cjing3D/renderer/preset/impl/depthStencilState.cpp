#include "renderer\preset\renderPreset.h"

namespace Cjing3D {


	void RenderPreset::SetupDepthStencilStates()
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
		desc.mDepthWriteMask = DEPTH_WRITE_MASK_ZERO;
		desc.mDepthFunc = COMPARISON_EQUAL;
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
			desc.mStencilEnable = false;
			desc.mDepthWriteMask = DEPTH_WRITE_MASK_ZERO;
			desc.mDepthFunc = COMPARISON_GREATER_EQUAL;

			const auto result = mDevice.CreateDepthStencilState(desc, *mDepthStencilStates[DepthStencilStateID::DepthStencilStateID_DepthRead]);
			Debug::ThrowIfFailed(result, "Failed to create greater equal depthStencilState", result);
		}
		{
			desc.mDepthEnable = true;
			desc.mStencilEnable = false;
			desc.mDepthWriteMask = DEPTH_WRITE_MASK_ZERO;
			desc.mDepthFunc = COMPARISON_EQUAL;

			const auto result = mDevice.CreateDepthStencilState(desc, *mDepthStencilStates[DepthStencilStateID::DepthStencilStateID_DepthReadEqual]);
			Debug::ThrowIfFailed(result, "Failed to create State:DepthStencilStateID_DepthReadEqual", result);
		}
		{
			desc.mDepthEnable = true;
			desc.mDepthWriteMask = DEPTH_WRITE_MASK_ALL;
			desc.mDepthFunc = COMPARISON_GREATER;
			desc.mStencilEnable = false;

			const auto result = mDevice.CreateDepthStencilState(desc, *mDepthStencilStates[DepthStencilStateID::DepthStencilStateID_Shadow]);
			Debug::ThrowIfFailed(result, "Failed to create shadow depthStencilState", result);
		}
	}
}