#include "renderPreset.h"
#include "renderer\RHI\device.h"
#include "renderer\RHI\rhiFactory.h"

namespace Cjing3D {

RenderPreset::RenderPreset(GraphicsDevice& device) :
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

RenderPreset::~RenderPreset()
{
}

void RenderPreset::Initialize()
{
	SetupDepthStencilStates();
	SetupBlendStates();
	SetupRasterizerStates();
	SetupSamplerStates();
	LoadConstantBuffers();
	LoadStructuredBuffers();
}

void RenderPreset::Uninitialize()
{
	for (int i = 0; i < DepthStencilStateID::DepthStencilStateID_Count; i++) {
		mDepthStencilStates[static_cast<U32>(i)].reset();
	}
	for (int i = 0; i < BlendStateID::BlendStateID_Count; i++) {
		mBlendStates[static_cast<U32>(i)].reset();
	}
	for (int i = 0; i < RasterizerStateID::RasterizerStateID_Count; i++) {
		mRasterizerStates[static_cast<U32>(i)].reset();
	}
	for (int i = 0; i < SamplerStateID::SamplerStateID_Count; i++) {
		mSamplerStates[static_cast<U32>(i)].reset();
	}
	for (U32 i = 0; i < ConstantBufferType_Count; i++) {
		mConstantBuffer[i].Clear();
	}
	for (U32 i = 0; i < StructuredBufferType_Count; i++) {
		mStructuredBuffer[i].Clear();
	}
	mCustomBufferMap.clear();

}

GPUBuffer& RenderPreset::GetOrCreateCustomBuffer(const StringID & name)
{
	auto it = mCustomBufferMap.find(name);
	if (it == mCustomBufferMap.end())
	{
		auto itTemp = mCustomBufferMap.emplace(name, GPUBuffer());
		it = itTemp.first;
	}
	return it->second;
}


}

