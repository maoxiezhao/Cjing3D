#include "rhiResourceManager.h"
#include "renderer\RHI\device.h"
#include "renderer\RHI\rhiFactory.h"
#include "helper\debug.h"

namespace Cjing3D {
namespace {
	HRESULT CreateWriteDynamicConstantBuffer(GraphicsDevice& device, GPUBuffer& buffer, U32 size)
	{
		GPUBufferDesc desc = {};
		desc.mUsage = USAGE_DYNAMIC;
		desc.mCPUAccessFlags = CPU_ACCESS_WRITE;
		desc.mBindFlags = BIND_CONSTANT_BUFFER;
		desc.mByteWidth = size;

		return device.CreateBuffer(&desc, buffer, nullptr);
	}

}

RhiResourceManager::RhiResourceManager(GraphicsDevice& device) :
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

RhiResourceManager::~RhiResourceManager()
{
}

void RhiResourceManager::Initialize()
{
	SetupDepthStencilStates();
	SetupBlendStates();
	SetupRasterizerStates();
	SetupSamplerStates();

	LoadConstantBuffers();
	LoadStructuredBuffers();
}

void RhiResourceManager::Uninitialize()
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

	mCustomBufferMap.clear();

	for (U32 i = 0; i < ConstantBufferType_Count; i++) {
		mConstantBuffer[i].Clear();
	}
	for (U32 i = 0; i < StructuredBufferType_Count; i++) {
		mStructuredBuffer[i].Clear();
	}
}

std::shared_ptr<DepthStencilState> RhiResourceManager::GetDepthStencilState(DepthStencilStateID id)
{
	Debug::CheckAssertion(id < DepthStencilStateID::DepthStencilStateID_Count);
	return mDepthStencilStates[static_cast<U32>(id)];
}

std::shared_ptr<BlendState> RhiResourceManager::GetBlendState(BlendStateID id)
{
	Debug::CheckAssertion(id < BlendStateID::BlendStateID_Count);
	return mBlendStates[static_cast<U32>(id)];
}

std::shared_ptr<RasterizerState> RhiResourceManager::GetRasterizerState(RasterizerStateID id)
{
	Debug::CheckAssertion(id < RasterizerStateID::RasterizerStateID_Count);
	return mRasterizerStates[static_cast<U32>(id)];
}

std::shared_ptr<SamplerState> RhiResourceManager::GetSamplerState(SamplerStateID id)
{
	Debug::CheckAssertion(id < SamplerStateID::SamplerStateID_Count);
	return mSamplerStates[static_cast<U32>(id)];
}

std::vector<std::shared_ptr<SamplerState>> RhiResourceManager::GetCommonSampleStates()
{
	return std::vector<std::shared_ptr<SamplerState>>();
}

void RhiResourceManager::SetupDepthStencilStates()
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

void RhiResourceManager::SetupBlendStates()
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
		desc.mRenderTarget[0].mBlendEnable = false;
		desc.mRenderTarget[0].mRenderTargetWriteMask = COLOR_WRITE_DISABLE;
		const auto result = mDevice.CreateBlendState(desc, *mBlendStates[BlendStateID::BlendStateID_ColorWriteDisable]);
		Debug::ThrowIfFailed(result, "Failed to create color write disable blendState", result);
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
	{
		// 用于2D场景渲染时使用，当渲染2D场景时，3D场景的显示以2D场景的Alpha作为Factor，BLEND_INV_SRC_ALPHA
		BlendStateDesc desc = {};
		desc.mAlphaToCoverageEnable = false;
		desc.mIndependentBlendEnable = false;
		desc.mRenderTarget[0].mBlendEnable = true;
		desc.mRenderTarget[0].mSrcBlend = BLEND_ONE;
		desc.mRenderTarget[0].mDstBlend = BLEND_INV_SRC_ALPHA;
		desc.mRenderTarget[0].mBlendOp = BLEND_OP_ADD;
		desc.mRenderTarget[0].mSrcBlendAlpha = BLEND_ONE;
		desc.mRenderTarget[0].mDstBlendAlpha = BLEND_ONE;
		desc.mRenderTarget[0].mBlendOpAlpha = BLEND_OP_ADD;
		desc.mRenderTarget[0].mRenderTargetWriteMask = COLOR_WRITE_ENABLE_ALL;
		const auto result = mDevice.CreateBlendState(desc, *mBlendStates[BlendStateID::BlendStateID_PreMultiplied]);
		Debug::ThrowIfFailed(result, "Failed to create transparent blendState", result);
	}
}

void RhiResourceManager::SetupRasterizerStates()
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
		Debug::ThrowIfFailed(result, "Failed to create shadow rasterizerState", result);
	}
}

void RhiResourceManager::SetupSamplerStates()
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

GPUBuffer& RhiResourceManager::GetOrCreateCustomBuffer(const StringID & name)
{
	auto it = mCustomBufferMap.find(name);
	if (it == mCustomBufferMap.end())
	{
		auto itTemp = mCustomBufferMap.emplace(name, GPUBuffer());
		it = itTemp.first;
	}
	return it->second;
}

void RhiResourceManager::LoadConstantBuffers()
{
	auto& device = mDevice;
	// common buffer
	{
		GPUBufferDesc desc = {};
		desc.mUsage = USAGE_DYNAMIC;	// fast update
		desc.mCPUAccessFlags = CPU_ACCESS_WRITE;
		desc.mBindFlags = BIND_CONSTANT_BUFFER;
		desc.mByteWidth = sizeof(CommonCB);

		const auto result = device.CreateBuffer(&desc, mConstantBuffer[ConstantBufferType_Common], nullptr);
		Debug::ThrowIfFailed(result, "failed to create common constant buffer:%08x", result);
		device.SetResourceName(mConstantBuffer[ConstantBufferType_Common], "CommonCB");
	}
	// Frame buffer
	{
		GPUBufferDesc desc = {};
		desc.mUsage = USAGE_DEFAULT;
		desc.mCPUAccessFlags = 0;
		desc.mBindFlags = BIND_CONSTANT_BUFFER;
		desc.mByteWidth = sizeof(FrameCB);

		const auto result = device.CreateBuffer(&desc, mConstantBuffer[ConstantBufferType_Frame], nullptr);
		Debug::ThrowIfFailed(result, "failed to create frame constant buffer:%08x", result);
		device.SetResourceName(mConstantBuffer[ConstantBufferType_Frame], "FrameCB");
	}
	// Camera buffer
	{
		GPUBuffer& buffer = mConstantBuffer[ConstantBufferType_Camera];
		const HRESULT result = CreateWriteDynamicConstantBuffer(device, buffer, sizeof(CameraCB));
		Debug::ThrowIfFailed(result, "failed to create camera constant buffer:%08x", result);
		device.SetResourceName(buffer, "CameraCB");
	}
	// image buffer
	{
		GPUBuffer& buffer = mConstantBuffer[ConstantBufferType_Image];
		const HRESULT result = CreateWriteDynamicConstantBuffer(device, buffer, sizeof(ImageCB));
		Debug::ThrowIfFailed(result, "failed to create image constant buffer:%08x", result);
		device.SetResourceName(buffer, "ImageCB");
	}
	// postprocess buffer
	{
		GPUBuffer& buffer = mConstantBuffer[ConstantBufferType_Postprocess];
		const HRESULT result = CreateWriteDynamicConstantBuffer(device, buffer, sizeof(PostprocessCB));
		Debug::ThrowIfFailed(result, "failed to create postprocess constant buffer:%08x", result);
		device.SetResourceName(buffer, "PostprocessCB");
	}
	// mipmap generate buffer
	{
		GPUBuffer& buffer = mConstantBuffer[ConstantBufferType_MipmapGenerate];
		const HRESULT result = CreateWriteDynamicConstantBuffer(device, buffer, sizeof(MipmapGenerateCB));
		Debug::ThrowIfFailed(result, "failed to create mipmap generate constant buffer:%08x", result);
		device.SetResourceName(buffer, "MipmapGenerateCB");
	}
	// CubeShadowCB
	{
		GPUBuffer& buffer = mConstantBuffer[ConstantBufferType_CubeMap];
		const HRESULT result = CreateWriteDynamicConstantBuffer(device, buffer, sizeof(CubeMapCB));
		Debug::ThrowIfFailed(result, "failed to create CubeShadowCB constant buffer:%08x", result);
		device.SetResourceName(buffer, "CubeShadowCB");
	}
	//
	{
		GPUBuffer& buffer = mConstantBuffer[ConstantBufferType_CSParams];
		const HRESULT result = CreateWriteDynamicConstantBuffer(device, buffer, sizeof(CSParamsCB));
		Debug::ThrowIfFailed(result, "failed to create CSParamsCB constant buffer:%08x", result);
		device.SetResourceName(buffer, "CSParamsCB");
	}
}

void RhiResourceManager::LoadStructuredBuffers()
{
	auto& device = mDevice;
	// shader light array
	{
		GPUBufferDesc desc = {};
		desc.mUsage = USAGE_DEFAULT;
		desc.mCPUAccessFlags = 0;
		desc.mBindFlags = BIND_SHADER_RESOURCE;
		desc.mMiscFlags = RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.mByteWidth = sizeof(ShaderLight) * SHADER_LIGHT_COUNT;
		desc.mStructureByteStride = sizeof(ShaderLight);

		const auto result = device.CreateBuffer(&desc, mStructuredBuffer[StructuredBufferType_ShaderLight], nullptr);
		Debug::ThrowIfFailed(result, "failed to create shader light buffer:%08x", result);
		device.SetResourceName(mStructuredBuffer[StructuredBufferType_ShaderLight], "ShaderLightArray");
	}
	// shader matrix array
	{
		GPUBufferDesc desc = {};
		desc.mUsage = USAGE_DEFAULT;
		desc.mCPUAccessFlags = 0;
		desc.mBindFlags = BIND_SHADER_RESOURCE;
		desc.mMiscFlags = RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.mByteWidth = sizeof(XMMATRIX) * SHADER_MATRIX_COUNT;
		desc.mStructureByteStride = sizeof(XMMATRIX);

		const auto result = device.CreateBuffer(&desc, mStructuredBuffer[StructuredBufferType_MatrixArray], nullptr);
		Debug::ThrowIfFailed(result, "failed to create shader matrix array buffer:%08x", result);
		device.SetResourceName(mStructuredBuffer[StructuredBufferType_MatrixArray], "ShaderMatrixArray");
	}
}
}

