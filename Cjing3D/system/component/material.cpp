#include "material.h"
#include "renderer\RHI\rhiFactory.h"

Cjing3D::MaterialComponent::MaterialComponent():
	Component(ComponentType_Material),
	mConstantBuffer(nullptr)
{
}

ShaderMaterial Cjing3D::MaterialComponent::CreateMaterialCB()
{
	ShaderMaterial sm;
	sm.baseColor = XMConvert(mBaseColor);
	sm.haveBaseColorMap = mBaseColorMap != nullptr ? 1 : 0;

	return sm;
}

void Cjing3D::MaterialComponent::SetupConstantBuffer(GraphicsDevice& device)
{
	if (mConstantBuffer == nullptr)
	{
		mConstantBuffer = std::make_unique<GPUBuffer>();

		const auto result = CreateDefaultConstantBuffer(device, *mConstantBuffer, sizeof(MaterialCB));
		Debug::ThrowIfFailed(result, "Failed to create material constant buffer:%08x", result);
	}
}
