#include "device.h"

namespace Cjing3D
{

GraphicsDevice::GraphicsDevice(GraphicsDeviceType type):
	mGraphicsDeviceType(type),
	mIsFullScreen(false),
	mIsMultithreadedRendering(false)
{
}

void GraphicsDevice::Initialize()
{
	mEmptyRhiState = std::make_shared<EmptyRhiState>();
}

void GraphicsDevice::Uninitialize()
{
	ProcessRemovedGPUResouces();

	for (GPUResource* resource : mGPUResource){
		if (resource != nullptr) {
			resource->Clear();
		}
	}

	mGPUResource.clear();
	mRemovedGPUResources.clear();
}

void GraphicsDevice::PresentBegin()
{
}

void GraphicsDevice::PresentEnd()
{
	ProcessRemovedGPUResouces();
}

void GraphicsDevice::AddGPUResource(GPUResource * resource)
{
	// �����ӵ�Resouce������ɾ�������У���ֱ�ӽ������е��Ƴ�����
	auto it = std::find(mRemovedGPUResources.begin(), mRemovedGPUResources.end(), resource);
	if (it != mRemovedGPUResources.end())
	{
		mRemovedGPUResources.erase(it);
		return;
	}

	if (std::find(mGPUResource.begin(), mGPUResource.end(), resource) == mGPUResource.end())
	{
		mGPUResource.push_back(resource);
	}
}

void GraphicsDevice::RemoveGPUResource(GPUResource * resource)
{
	if (std::find(mRemovedGPUResources.begin(), mRemovedGPUResources.end(), resource) == mRemovedGPUResources.end())
	{
		mRemovedGPUResources.push_back(resource);
	}
}

void GraphicsDevice::ProcessRemovedGPUResouces()
{
	if (mRemovedGPUResources.empty()) {
		return;
	}

	for (GPUResource* resource : mRemovedGPUResources) {
		mGPUResource.remove(resource);
	}
	mRemovedGPUResources.clear();
}

bool GraphicsDevice::CheckGraphicsFeatureSupport(GraphicsFeatureSupport::GraphicsFeature feature) const
{
	switch (feature)
	{
	case Cjing3D::GraphicsFeatureSupport::VIEWPORT_AND_RENDERTARGET_ARRAYINDEX_WITHOUT_GS:
		return mGraphicsFeatureSupport.viewportAndRenderTargetArrayIndexWithoutGS_;
		break;
	default:
		break;
	}
	return false;
}

U32 GraphicsDevice::GetFormatStride(FORMAT value) const
{
	switch (value)
	{

	case FORMAT_R32G32B32A32_FLOAT:
	case FORMAT_R32G32B32A32_UINT:
	case FORMAT_R32G32B32A32_SINT:
		return 16;

	case FORMAT_R32G32B32_FLOAT:
	case FORMAT_R32G32B32_UINT:
	case FORMAT_R32G32B32_SINT:
		return 12;

	case FORMAT_R16G16B16A16_FLOAT:
	case FORMAT_R16G16B16A16_UNORM:
	case FORMAT_R16G16B16A16_UINT:
	case FORMAT_R16G16B16A16_SNORM:
	case FORMAT_R16G16B16A16_SINT:
		return 8;

	case FORMAT_R32G32_FLOAT:
	case FORMAT_R32G32_UINT:
	case FORMAT_R32G32_SINT:
	case FORMAT_R32G8X24_TYPELESS:
	case FORMAT_D32_FLOAT_S8X24_UINT:
		return 8;

	case FORMAT_R10G10B10A2_UNORM:
	case FORMAT_R10G10B10A2_UINT:
	case FORMAT_R11G11B10_FLOAT:
	case FORMAT_R8G8B8A8_UNORM:
	case FORMAT_R8G8B8A8_UNORM_SRGB:
	case FORMAT_R8G8B8A8_UINT:
	case FORMAT_R8G8B8A8_SNORM:
	case FORMAT_R8G8B8A8_SINT:
	case FORMAT_B8G8R8A8_UNORM:
	case FORMAT_B8G8R8A8_UNORM_SRGB:
	case FORMAT_R16G16_FLOAT:
	case FORMAT_R16G16_UNORM:
	case FORMAT_R16G16_UINT:
	case FORMAT_R16G16_SNORM:
	case FORMAT_R16G16_SINT:
	case FORMAT_R32_TYPELESS:
	case FORMAT_D32_FLOAT:
	case FORMAT_R32_FLOAT:
	case FORMAT_R32_UINT:
	case FORMAT_R32_SINT:
	case FORMAT_R24G8_TYPELESS:
	case FORMAT_D24_UNORM_S8_UINT:
		return 4;

	case FORMAT_R8G8_UNORM:
	case FORMAT_R8G8_UINT:
	case FORMAT_R8G8_SNORM:
	case FORMAT_R8G8_SINT:
	case FORMAT_R16_TYPELESS:
	case FORMAT_R16_FLOAT:
	case FORMAT_D16_UNORM:
	case FORMAT_R16_UNORM:
	case FORMAT_R16_UINT:
	case FORMAT_R16_SNORM:
	case FORMAT_R16_SINT:
		return 2;

	case FORMAT_R8_UNORM:
	case FORMAT_R8_UINT:
	case FORMAT_R8_SNORM:
	case FORMAT_R8_SINT:
		return 1;

	default:
		break;
	}

	return 16;
}

bool GraphicsDevice::IsFormatUnorm(FORMAT value) const
{
	switch (value)
	{
	case FORMAT_R16G16B16A16_UNORM:
	case FORMAT_R10G10B10A2_UNORM:
	case FORMAT_R8G8B8A8_UNORM:
	case FORMAT_R8G8B8A8_UNORM_SRGB:
	case FORMAT_B8G8R8A8_UNORM:
	case FORMAT_B8G8R8A8_UNORM_SRGB:
	case FORMAT_R16G16_UNORM:
	case FORMAT_D24_UNORM_S8_UINT:
	case FORMAT_R8G8_UNORM:
	case FORMAT_D16_UNORM:
	case FORMAT_R16_UNORM:
	case FORMAT_R8_UNORM:
		return true;
	}

	return false;
}

void GraphicsDevice::CreatePipelineState(PipelineStateDesc& desc, PipelineState& state)
{
	state.mRhiState = mEmptyRhiState;
	state.mDesc = desc;
}

}
