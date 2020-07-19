#include "renderPreset.h"

namespace Cjing3D {

	void RenderPreset::SetupRasterizerStates()
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
			Debug::ThrowIfFailed(result, "Failed to create front rasterizerState", result);
		}

		{
			RasterizerStateDesc desc = {};
			desc.mFillMode = FILL_SOLID;
			desc.mCullMode = CULL_NONE;
			desc.mFrontCounterClockwise = false;
			desc.mDepthBias = 0;
			desc.mDepthBiasClamp = 0;
			desc.mSlopeScaleDepthBias = 0;
			desc.mDepthClipEnable = false;
			desc.mMultisampleEnable = false;
			desc.mAntialiaseLineEnable = false;
			desc.mConservativeRasterizationEnable = false;

			const auto result = mDevice.CreateRasterizerState(desc, *mRasterizerStates[RasterizerStateID::RasterizerStateID_Image]);
			Debug::ThrowIfFailed(result, "Failed to create image rasterizerState", result);
		}

		{
			RasterizerStateDesc desc = {};
			desc.mFillMode = FILL_SOLID;
			desc.mCullMode = CULL_BACK;
			desc.mFrontCounterClockwise = true;
			desc.mDepthBias = 0;
			desc.mDepthBiasClamp = 0;
			desc.mSlopeScaleDepthBias = -2.0f;
			desc.mDepthClipEnable = true;
			desc.mMultisampleEnable = false;
			desc.mAntialiaseLineEnable = false;
			desc.mConservativeRasterizationEnable = false;

			const auto result = mDevice.CreateRasterizerState(desc, *mRasterizerStates[RasterizerStateID::RasterizerStateID_Shadow]);
			Debug::ThrowIfFailed(result, "Failed to create shadow rasterizerState", result);
		}

		{
			RasterizerStateDesc desc = {};
			desc.mFillMode = FILL_WIREFRAME;
			desc.mCullMode = CULL_BACK;
			desc.mFrontCounterClockwise = true;
			desc.mDepthBias = 0;
			desc.mDepthBiasClamp = 0;
			desc.mSlopeScaleDepthBias = 0;
			desc.mDepthClipEnable = true;
			desc.mMultisampleEnable = false;
			desc.mAntialiaseLineEnable = false;
			desc.mConservativeRasterizationEnable = false;

			const auto result = mDevice.CreateRasterizerState(desc, *mRasterizerStates[RasterizerStateID::RasterizerStateID_WireFrame]);
			Debug::ThrowIfFailed(result, "Failed to create shadow rasterizerState", result);
		}
		{
			RasterizerStateDesc desc = {};
			desc.mFillMode = FILL_SOLID;
			desc.mCullMode = CULL_FRONT;
			desc.mFrontCounterClockwise = true;
			desc.mDepthBias = 0;
			desc.mDepthBiasClamp = 0;
			desc.mSlopeScaleDepthBias = 0;
			desc.mDepthClipEnable = false;
			desc.mMultisampleEnable = false;
			desc.mAntialiaseLineEnable = false;
			desc.mConservativeRasterizationEnable = false;

			const auto result = mDevice.CreateRasterizerState(desc, *mRasterizerStates[RasterizerStateID::RasterizerStateID_Sky]);
			Debug::ThrowIfFailed(result, "Failed to create sky rasterizerState", result);
		}
		{
			RasterizerStateDesc desc = {};
			desc.mFillMode = FILL_WIREFRAME;
			desc.mCullMode = CULL_NONE;
			desc.mFrontCounterClockwise = true;
			desc.mDepthBias = 0;
			desc.mDepthBiasClamp = 0;
			desc.mSlopeScaleDepthBias = 0;
			desc.mDepthClipEnable = true;
			desc.mMultisampleEnable = false;
			desc.mAntialiaseLineEnable = false;
			desc.mConservativeRasterizationEnable = false;
			desc.mAntialiaseLineEnable = true;

			const auto result = mDevice.CreateRasterizerState(desc, *mRasterizerStates[RasterizerStateID::RasterizerStateID_WireFrame_DoubleSided]);
			Debug::ThrowIfFailed(result, "Failed to create wire doubleSided rasterizerState", result);
		}
	}
}