#include "resourceManager.h"
#include "helper\fileSystem.h"
#include "utils\utilsCommon.h"
#include "renderer\renderer.h"
#include "core\systemContext.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "utils\stb_image.h"

#define TINYDDSLOADER_IMPLEMENTATION
#include "utils\tinyddsloader.h"

namespace Cjing3D
{
void ResourceManager::Initialize()
{
	AddStandardResourceDirectory(Resrouce_VertexShader, "Shaders/");
	AddStandardResourceDirectory(Resrouce_PixelShader, "Shaders/");
	AddStandardResourceDirectory(Resource_Model, "Models/");

}

void ResourceManager::Uninitialize()
{
}

void ResourceManager::AddStandardResourceDirectory(Resource_Type type, const std::string & path)
{
	mResourceDirectories[type] = path;
}

const std::string & ResourceManager::GetStandardResourceDirectory(Resource_Type type) const
{
	auto findIt = mResourceDirectories.find(type);
	if (findIt != mResourceDirectories.end())
	{
		return findIt->second;
	}
}

ResourcePtr ResourceManager::GetOrCreateByType(const StringID & name, Resource_Type type)
{
	return ResourcePtr();
}

void ResourceManager::LoadTextrueFromFilePath(const std::filesystem::path & filePath, RhiTexture2D & texture)
{
	std::wstring extension(filePath.extension());
	if (extension == L".dds")
	{
		tinyddsloader::DDSFile ddsFile;
		auto result = ddsFile.Load(filePath.generic_string().c_str());
		if (result == tinyddsloader::Result::Success)
		{
			// todo
		}
	}
	else
	{
		const std::string path = filePath.generic_string();
		size_t length = 0;
		unsigned char* data = (unsigned char* )FileData::ReadFileBytes(path, length);

		const int channelCount = 4;
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
			desc.mFormat = FORMAT_R8G8B8A8_UNORM;
			desc.mBindFlags = BIND_UNORDERED_ACCESS | BIND_SHADER_RESOURCE;
		
			std::vector<SubresourceData> resourceData(desc.mMipLevels);
			resourceData[0].mSysMem = rgb;
			resourceData[0].mSysMemPitch = width * channelCount;
			{
				const auto result = renderer.GetDevice().CreateTexture2D(&desc, resourceData.data(), texture);
				Debug::ThrowIfFailed(result, "Failed to create render target:%08x", result);

				renderer.GetDevice().SetResourceName(texture, path.c_str());
			}
		}

		stbi_image_free(rgb);
		
		if (data != nullptr) {
			delete[] data;
		}
	}
}


}