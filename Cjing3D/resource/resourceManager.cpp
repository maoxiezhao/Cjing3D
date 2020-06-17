#include "resourceManager.h"
#include "helper\fileSystem.h"
#include "utils\utilsCommon.h"
#include "renderer\renderer.h"
#include "core\systemContext.hpp"
#include "audio\audio.h"

#include "utils\stb_image_include.h"

#define TINYDDSLOADER_IMPLEMENTATION
#include "utils\tinyddsloader.h"

namespace Cjing3D
{
void ResourceManager::Initialize()
{
	AddStandardResourceDirectory(Resource_Shader, "Shaders/");
	AddStandardResourceDirectory(Resource_Model, "Models/");
	AddStandardResourceDirectory(Resource_Sound, "Sounds/");
}

void ResourceManager::Uninitialize()
{
}

void ResourceManager::AddStandardResourceDirectory(ResourceType type, const std::string & path)
{
	mResourceDirectories[type] = path;
}

const std::string & ResourceManager::GetStandardResourceDirectory(ResourceType type) const
{
	auto findIt = mResourceDirectories.find(type);
	if (findIt != mResourceDirectories.end())
	{
		return findIt->second;
	}
}

ResourcePtr ResourceManager::GetOrCreateByType(const StringID & name, ResourceType type)
{
	return ResourcePtr();
}

void ResourceManager::LoadTextureFromFilePath(const std::filesystem::path& filePath, Texture2D& texture, FORMAT textureFormat, U32 channelCount, U32 bindFlag, bool generateMipmap)
{
	Logger::Info("LoadTextureFromFilePath:" + filePath.string());

	std::wstring extension(filePath.extension());
	if (extension == L".dds")
	{
		const std::string path = filePath.generic_string();
		if (!FileData::IsFileExists(path))
		{
			Debug::Warning("Cannot open file [" + path + "]");
			return;
		}

		size_t size = 0;
		char* data = FileData::ReadFileBytes(path, size);
		if (data == nullptr) {
			Debug::Warning("Cannot open file [" + path + "]");
			return;
		}

		tinyddsloader::DDSFile ddsFile;
		auto result = ddsFile.Load((const uint8_t*)data, size);
		if (result == tinyddsloader::Result::Success)
		{
			TextureDesc desc = {};
			desc.mWidth = ddsFile.GetWidth();
			desc.mHeight = ddsFile.GetHeight();
			desc.mDepth = ddsFile.GetDepth();
			desc.mArraySize = ddsFile.GetArraySize();
			desc.mMipLevels = ddsFile.GetMipCount();
			desc.mFormat = textureFormat;
			desc.mBindFlags = bindFlag;
			desc.mUsage = USAGE_IMMUTABLE;

			if (ddsFile.IsCubemap()) {
				desc.mMiscFlags |= RESOURCE_MISC_TEXTURECUBE;
			}

			auto ddsFormat = ddsFile.GetFormat();
			switch (ddsFormat)
			{
				case tinyddsloader::DDSFile::DXGIFormat::B8G8R8A8_UNorm: desc.mFormat = FORMAT_B8G8R8A8_UNORM; break;
				case tinyddsloader::DDSFile::DXGIFormat::B8G8R8A8_UNorm_SRGB: desc.mFormat = FORMAT_B8G8R8A8_UNORM_SRGB; break;
				case tinyddsloader::DDSFile::DXGIFormat::R8G8B8A8_UNorm: desc.mFormat = FORMAT_R8G8B8A8_UNORM; break;
				case tinyddsloader::DDSFile::DXGIFormat::R8G8B8A8_UNorm_SRGB: desc.mFormat = FORMAT_R8G8B8A8_UNORM_SRGB; break;
				case tinyddsloader::DDSFile::DXGIFormat::R8G8B8A8_UInt: desc.mFormat = FORMAT_R8G8B8A8_UINT; break;
				case tinyddsloader::DDSFile::DXGIFormat::R8G8B8A8_SNorm: desc.mFormat = FORMAT_R8G8B8A8_SNORM; break;
				case tinyddsloader::DDSFile::DXGIFormat::R8G8B8A8_SInt: desc.mFormat = FORMAT_R8G8B8A8_SINT; break;
				case tinyddsloader::DDSFile::DXGIFormat::R16G16_Float: desc.mFormat = FORMAT_R16G16_FLOAT; break;
				case tinyddsloader::DDSFile::DXGIFormat::R16G16_UNorm: desc.mFormat = FORMAT_R16G16_UNORM; break;
				case tinyddsloader::DDSFile::DXGIFormat::R16G16_UInt: desc.mFormat = FORMAT_R16G16_UINT; break;
				case tinyddsloader::DDSFile::DXGIFormat::R16G16_SNorm: desc.mFormat = FORMAT_R16G16_SNORM; break;
				case tinyddsloader::DDSFile::DXGIFormat::R16G16_SInt: desc.mFormat = FORMAT_R16G16_SINT; break;
				case tinyddsloader::DDSFile::DXGIFormat::D32_Float: desc.mFormat = FORMAT_D32_FLOAT; break;
				case tinyddsloader::DDSFile::DXGIFormat::R32_Float: desc.mFormat = FORMAT_R32_FLOAT; break;
				case tinyddsloader::DDSFile::DXGIFormat::R32_UInt: desc.mFormat = FORMAT_R32_UINT; break;
				case tinyddsloader::DDSFile::DXGIFormat::R32_SInt: desc.mFormat = FORMAT_R32_SINT; break;
				case tinyddsloader::DDSFile::DXGIFormat::R8G8_UNorm: desc.mFormat = FORMAT_R8G8_UNORM; break;
				case tinyddsloader::DDSFile::DXGIFormat::R8G8_UInt: desc.mFormat = FORMAT_R8G8_UINT; break;
				case tinyddsloader::DDSFile::DXGIFormat::R8G8_SNorm: desc.mFormat = FORMAT_R8G8_SNORM; break;
				case tinyddsloader::DDSFile::DXGIFormat::R8G8_SInt: desc.mFormat = FORMAT_R8G8_SINT; break;
				case tinyddsloader::DDSFile::DXGIFormat::R16_Float: desc.mFormat = FORMAT_R16_FLOAT; break;
				case tinyddsloader::DDSFile::DXGIFormat::D16_UNorm: desc.mFormat = FORMAT_D16_UNORM; break;
				case tinyddsloader::DDSFile::DXGIFormat::R16_UNorm: desc.mFormat = FORMAT_R16_UNORM; break;
				case tinyddsloader::DDSFile::DXGIFormat::R16_UInt: desc.mFormat = FORMAT_R16_UINT; break;
				case tinyddsloader::DDSFile::DXGIFormat::R16_SNorm: desc.mFormat = FORMAT_R16_SNORM; break;
				case tinyddsloader::DDSFile::DXGIFormat::R16_SInt: desc.mFormat = FORMAT_R16_SINT; break;
				case tinyddsloader::DDSFile::DXGIFormat::R8_UNorm: desc.mFormat = FORMAT_R8_UNORM; break;
				case tinyddsloader::DDSFile::DXGIFormat::R8_UInt: desc.mFormat = FORMAT_R8_UINT; break;
				case tinyddsloader::DDSFile::DXGIFormat::R8_SNorm: desc.mFormat = FORMAT_R8_SNORM; break;
				case tinyddsloader::DDSFile::DXGIFormat::R8_SInt: desc.mFormat = FORMAT_R8_SINT; break;
				case tinyddsloader::DDSFile::DXGIFormat::BC1_UNorm: desc.mFormat = FORMAT_BC1_UNORM; break;
			default:
				Debug::Warning("Invalid dds texture format [" + std::to_string(static_cast<U32>(ddsFormat)) + ", " + path + "]");
				break;
			}

			std::vector<SubresourceData> resourceData;
			for (U32 index = 0; index < desc.mArraySize; index++)
			{
				for (U32 mipLevle = 0; mipLevle < desc.mMipLevels; mipLevle++)
				{
					auto imageData = ddsFile.GetImageData(mipLevle, index);
					SubresourceData subresourceData;
					subresourceData.mSysMem = imageData->m_mem;
					subresourceData.mSysMemPitch = imageData->m_memPitch;
					subresourceData.mSysMemSlicePitch = imageData->m_memSlicePitch;

					resourceData.push_back(subresourceData);
				}
			}

			// only support texture2D now.
			if (ddsFile.GetTextureDimension() != tinyddsloader::DDSFile::TextureDimension::Texture2D) {
				return;
			}

			Renderer& renderer = GlobalGetSubSystem<Renderer>();
			const auto result = renderer.GetDevice().CreateTexture2D(&desc, resourceData.data(), texture);
			Debug::ThrowIfFailed(result, "Failed to create texture:%08x", result);
			renderer.GetDevice().SetResourceName(texture, path.c_str());
		}

		SAFE_DELETE_ARRAY(data);
	}
	else
	{
		const std::string path = filePath.generic_string();
		size_t length = 0;
		unsigned char* data = (unsigned char*)FileData::ReadFileBytes(path, length);

		int width, height, bpp;
		unsigned char* rgb = stbi_load_from_memory(data, length, &width, &height, &bpp, channelCount);
		if (rgb != nullptr)
		{
			SystemContext& systemContext = SystemContext::GetSystemContext();
			Renderer& renderer = systemContext.GetSubSystem<Renderer>();
			GraphicsDevice& device = renderer.GetDevice();

			TextureDesc desc = {};
			desc.mWidth = width;
			desc.mHeight = height;
			desc.mArraySize = 1;
			desc.mFormat = textureFormat;
			desc.mBindFlags = bindFlag;
			desc.mMipLevels = generateMipmap ? static_cast<U32>(std::log2(std::max(width, height))) : 1;

			std::vector<SubresourceData> resourceData(desc.mMipLevels);

			// 对于非dds纹理加载，需要自建mipmap
			U32 mipWidth = width;
			for (int mipLevel = 0; mipLevel < desc.mMipLevels; mipLevel++)
			{
				resourceData[mipLevel].mSysMem = rgb;
				resourceData[mipLevel].mSysMemPitch = mipWidth * channelCount;

				mipWidth = std::max(1u, mipWidth / 2);
			}

			const auto result = renderer.GetDevice().CreateTexture2D(&desc, resourceData.data(), texture);
			Debug::ThrowIfFailed(result, "Failed to create texture:%08x", result);
			renderer.GetDevice().SetResourceName(texture, path.c_str());

			// 创建各个subresource的srv和uav,用于deferred generate mipmap
			for (int mipLevel = 0; mipLevel < desc.mMipLevels; mipLevel++)
			{
				renderer.GetDevice().CreateShaderResourceView(texture, 0, -1, mipLevel, 1);
				renderer.GetDevice().CreateUnordereddAccessView(texture, mipLevel);
			}

			if (desc.mMipLevels > 1) {
				renderer.AddDeferredTextureMipGen(texture);
			}
		}

		stbi_image_free(rgb);

		if (data != nullptr) {
			delete[] data;
		}
	}
}

void ResourceManager::LoadSoundFromFilePath(const std::filesystem::path& filePath, SoundResource& soundResource)
{
	Logger::Info("LoadSoundFromFilePath:" + filePath.string());

	Audio::AudioManager& manger = GlobalGetSubSystem<Audio::AudioManager>();
	if (!manger.LoadSound(filePath.string(), soundResource.mSound))
	{
		soundResource.mSound.Clear();
		Debug::Warning("LoadSoundFromFilePath failed:" + filePath.string());
		return;
	}
}


}