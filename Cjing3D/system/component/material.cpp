#include "material.h"
#include "renderer\renderer.h"
#include "renderer\RHI\rhiFactory.h"
#include "resource\resourceManager.h"

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
	sm.haveNormalMap = mNormalMap != nullptr ? 1 : 0;
	sm.haveSurfaceMap = mSurfaceMap != nullptr ? 1 : 0;
	sm.useVertexColor = IsUsingVertexColors() ? 1 : 0;

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

void Cjing3D::MaterialComponent::Serialize(Archive& archive, U32 seed)
{
	archive >> mBaseColor;
	archive >> mRoughness;
	archive >> mMetalness;

	archive >> mBaseColorMapName;
	archive >> mSurfaceMapName;
	archive >> mNormalMapName;

	SystemContext& systemContext = SystemContext::GetSystemContext();
	Renderer& renderer = systemContext.GetSubSystem<Renderer>();
	ResourceManager& resourceManager = renderer.GetResourceManager();


	const std::string parentPath =  archive.GetDirectory();
	if (mBaseColorMapName.empty() == false)
	{
		auto texPath = FileData::ConvertToAvailablePath(parentPath + mBaseColorMapName);
		mBaseColorMap = resourceManager.GetOrCreate<RhiTexture2D>(StringID(texPath));
	}
	if (mNormalMapName.empty() == false)
	{
		auto texPath = FileData::ConvertToAvailablePath(parentPath + mNormalMapName);
		mNormalMap = resourceManager.GetOrCreate<RhiTexture2D>(StringID(texPath));
	}
	if (mSurfaceMapName.empty() == false)
	{
		auto texPath = FileData::ConvertToAvailablePath(parentPath + mSurfaceMapName);
		mSurfaceMap = resourceManager.GetOrCreate<RhiTexture2D>(StringID(texPath));
	}

	// renderer will setup render data
	SetIsDirty(true);
}

void Cjing3D::MaterialComponent::Unserialize(Archive& archive) const
{
	archive << mBaseColor;
	archive << mRoughness;
	archive << mMetalness;

	archive << mBaseColorMapName;
	archive << mSurfaceMapName;
	archive << mNormalMapName;
}
