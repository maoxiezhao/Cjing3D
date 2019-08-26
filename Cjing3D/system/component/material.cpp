#include "material.h"
#include "renderer\RHI\rhiFactory.h"

Cjing3D::MaterialComponent::MaterialComponent():
	Component(ComponentType_Material),
	mConstantBuffer(nullptr)
{
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
