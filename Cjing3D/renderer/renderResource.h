#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\RHI\rhiStructure.h"
#include "renderer\includeD3D11.h"

namespace Cjing3D
{
	class DepthStencilState
	{
	public:
		DepthStencilState() : mState() {};

		DepthStencilStateDesc GetDesc()const { return mDesc; }
		ID3D11DepthStencilState& GetState() { return *mState.Get(); }

	private:
		DepthStencilStateDesc mDesc;
		ComPtr<ID3D11DepthStencilState> mState;
	};

	class BlendState
	{
	public:
		BlendState() : mState() {};

		BlendStateDesc GetDesc()const { return mDesc; }
		ID3D11BlendState& GetState() { return *mState.Get(); }

	private:
		BlendStateDesc mDesc;
		ComPtr<ID3D11BlendState> mState;
	};
}