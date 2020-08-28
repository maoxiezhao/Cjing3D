#include "material.h"
#include "renderer\renderer.h"
#include "renderer\RHI\rhiFactory.h"
#include "renderer\textureHelper.h"
#include "resource\resourceManager.h"

Cjing3D::MaterialComponent::MaterialComponent():
	Component(ComponentType_Material)
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
	if (!mConstantBuffer.IsValid())
	{
		const auto result = CreateDefaultConstantBuffer(device, mConstantBuffer, sizeof(MaterialCB));
		Debug::ThrowIfFailed(result, "Failed to create material constant buffer:%08x", result);
	}
}

bool Cjing3D::MaterialComponent::LoadBaseColorMap(const std::string& filePath)
{
	auto resourceManager = GetGlobalContext().gResourceManager;
	mBaseColorMapName = filePath;
	mBaseColorMap = resourceManager->GetOrCreate<TextureResource>(StringID(filePath));
	return true;
}

bool Cjing3D::MaterialComponent::LoadNormalMap(const std::string& filePath)
{
	auto resourceManager = GetGlobalContext().gResourceManager;
	mNormalMapName = filePath;
	mNormalMap = resourceManager->GetOrCreate<TextureResource>(StringID(filePath));
	return true;
}

bool Cjing3D::MaterialComponent::LoadSurfaceMap(const std::string& filePath)
{
	auto resourceManager = GetGlobalContext().gResourceManager;
	mSurfaceMapName = filePath;
	mSurfaceMap = resourceManager->GetOrCreate<TextureResource>(StringID(filePath));
	return true;
}

const Cjing3D::Texture2D* Cjing3D::MaterialComponent::GetBaseColorMap() const
{
	return mBaseColorMap != nullptr ? mBaseColorMap->mTexture : TextureHelper::GetWhite();
}

const Cjing3D::Texture2D* Cjing3D::MaterialComponent::GetNormalMap() const
{
	return mNormalMap != nullptr ? mNormalMap->mTexture : nullptr;
}

const Cjing3D::Texture2D* Cjing3D::MaterialComponent::GetSurfaceMap() const
{
	return mSurfaceMap != nullptr ? mSurfaceMap->mTexture : TextureHelper::GetWhite();
}

Cjing3D::Texture2D* Cjing3D::MaterialComponent::GetBaseColorMapPtr() const
{
	return mBaseColorMap != nullptr ? mBaseColorMap->mTexture : nullptr;
}

Cjing3D::Texture2D* Cjing3D::MaterialComponent::GetNormalMapPtr() const
{
	return mNormalMap != nullptr ? mNormalMap->mTexture : nullptr;
}

Cjing3D::Texture2D* Cjing3D::MaterialComponent::GetSurfaceMapPtr() const
{
	return mSurfaceMap != nullptr ? mSurfaceMap->mTexture : nullptr;
}

void Cjing3D::MaterialComponent::Serialize(Archive& archive, U32 seed)
{
	archive >> mBaseColor;
	archive >> mRoughness;
	archive >> mMetalness;

	archive >> mBaseColorMapName;
	archive >> mSurfaceMapName;
	archive >> mNormalMapName;
	archive >> mBlendType;
	archive >> mIsCastingShadow;
	archive >> mAlphaCutRef;

	auto resourceManager = GetGlobalContext().gResourceManager;
	const std::string parentPath =  archive.GetDirectory();
	if (mBaseColorMapName.empty() == false)
	{
		auto texPath = FileData::ConvertToAvailablePath(parentPath + mBaseColorMapName);
		mBaseColorMap = resourceManager->GetOrCreate<TextureResource>(StringID(texPath));
	}
	if (mNormalMapName.empty() == false)
	{
		auto texPath = FileData::ConvertToAvailablePath(parentPath + mNormalMapName);
		mNormalMap = resourceManager->GetOrCreate<TextureResource>(StringID(texPath));
	}
	if (mSurfaceMapName.empty() == false)
	{
		auto texPath = FileData::ConvertToAvailablePath(parentPath + mSurfaceMapName);
		mSurfaceMap = resourceManager->GetOrCreate<TextureResource>(StringID(texPath));
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
	archive << mBlendType;
	archive << mIsCastingShadow;
	archive << mAlphaCutRef;
}
