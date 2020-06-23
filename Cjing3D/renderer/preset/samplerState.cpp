#include "renderPreset.h"
#include "renderer\RHI\rhiFactory.h"

namespace Cjing3D {

	void RenderPreset::SetupSamplerStates()
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

		{
			const auto result = CreateDefaultSamplerState(mDevice, *mSamplerStates[SamplerStateID_ANISOTROPIC],
				FILTER_ANISOTROPIC, TEXTURE_ADDRESS_WRAP, COMPARISON_NEVER, 16);
			Debug::ThrowIfFailed(result, "Failed to create SamplerStateID_ANISOTROPIC samplerState", result);
		}

		{
			const auto result = CreateDefaultSamplerState(mDevice, *mSamplerStates[SamplerStateID_Comparision_depth],
				FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, TEXTURE_ADDRESS_CLAMP, COMPARISON_GREATER_EQUAL);
			Debug::ThrowIfFailed(result, "Failed to create SamplerStateID_Comparision_depth samplerState", result);
		}
	}

}