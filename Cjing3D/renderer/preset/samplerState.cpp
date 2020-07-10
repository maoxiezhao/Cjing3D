#include "renderPreset.h"
#include "renderer\RHI\rhiFactory.h"

namespace Cjing3D {

	void RenderPreset::SetupSamplerStates()
	{
		{
			const auto result = CreateDefaultSamplerState(mDevice, *mSamplerStates[SamplerStateID_PointClamp],
				FILTER_MIN_MAG_MIP_POINT, TEXTURE_ADDRESS_CLAMP, COMPARISON_NEVER);
			Debug::ThrowIfFailed(result, "Failed to create pointClampAlways samplerState", result);
		}
		{
			const auto result = CreateDefaultSamplerState(mDevice, *mSamplerStates[SamplerStateID_LinearClamp],
				FILTER_MIN_MAG_LINEAR_MIP_POINT, TEXTURE_ADDRESS_CLAMP, COMPARISON_NEVER);
			Debug::ThrowIfFailed(result, "Failed to create pointClampAlways samplerState", result);
		}
		{
			const auto result = CreateDefaultSamplerState(mDevice, *mSamplerStates[SamplerStateID_Anisotropic],
				FILTER_ANISOTROPIC, TEXTURE_ADDRESS_WRAP, COMPARISON_NEVER, 16);
			Debug::ThrowIfFailed(result, "Failed to create SamplerStateID_Anisotropic samplerState", result);
		}

		{
			const auto result = CreateDefaultSamplerState(mDevice, *mSamplerStates[SamplerStateID_ComparisionDepth],
				FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, TEXTURE_ADDRESS_CLAMP, COMPARISON_GREATER_EQUAL);
			Debug::ThrowIfFailed(result, "Failed to create SamplerStateID_ComparisionDepth samplerState", result);
		}

		{
			SamplerDesc desc = {};
			// ������ʽ
			desc.mFilter = FILTER_MIN_MAG_LINEAR_MIP_POINT;
			// ����Ѱַģʽ
			desc.mAddressU = TEXTURE_ADDRESS_BORDER;
			desc.mAddressV = TEXTURE_ADDRESS_BORDER;
			desc.mAddressW = TEXTURE_ADDRESS_BORDER;
			// �߽���ɫ
			desc.mBorderColor[0] = 0;
			desc.mBorderColor[1] = 0;
			desc.mBorderColor[2] = 0;
			desc.mBorderColor[3] = 0;
			// 
			desc.mComparisonFunc = COMPARISON_NEVER;

			HRESULT result = mDevice.CreateSamplerState(&desc, *mSamplerStates[SamplerStateID_Font]);
			Debug::ThrowIfFailed(result, "Failed to create Sfont samplerState", result);
		}
	}

}