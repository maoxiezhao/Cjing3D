#include "textureHelper.h"
#include "renderer.h"
#include "helper\logger.h"

#include <unordered_map>

namespace Cjing3D
{
	namespace TextureHelper
	{
		// todo
		std::unordered_map<unsigned long, Texture2D*> mColorTextureMap;
	}

	void TextureHelper::Initialize()
	{

		Logger::Info("TextureHelper initialize.");
	}

	void TextureHelper::Uninitialize()
	{
		for (auto& key : mColorTextureMap)
		{
			if (key.second != nullptr) {
				delete key.second;
			}
		}
		mColorTextureMap.clear();

		Logger::Info("TextureHelper unInitialize.");
	}

	Texture2D* TextureHelper::GetWhite()
	{
		return GetColor(Color4::White());
	}

	Texture2D* TextureHelper::GetColor(const Color4& color)
	{
		auto it = mColorTextureMap.find(color.mRGBA);
		if (it != mColorTextureMap.end()) {
			return it->second;
		}

		static const int dataLength = 4;
		uint8_t data[dataLength] = {
			color.GetR(),
			color.GetG(),
			color.GetB(),
			color.GetA()
		};

		Texture2D* texture = new Texture2D();
		HRESULT result = CreateTexture(*texture, data, 1, 1, FORMAT_R8G8B8A8_UNORM);
		if (FAILED(result))
		{
			delete texture;
			return nullptr;
		}

		SystemContext& systemContext = SystemContext::GetSystemContext();
		Renderer& renderer = systemContext.GetSubSystem<Renderer>();
		renderer.GetDevice().SetResourceName(*texture, "mColorTexture");

		mColorTextureMap[color.mRGBA] = texture;

		return texture;
	}

	HRESULT TextureHelper::CreateTexture(Texture2D& texture, const uint8_t* data, U32 width, U32 height, FORMAT format)
	{
		SystemContext& systemContext = SystemContext::GetSystemContext();
		Renderer& renderer = systemContext.GetSubSystem<Renderer>();
		GraphicsDevice& device = renderer.GetDevice();

		TextureDesc desc = {};
		desc.mWidth = width;
		desc.mHeight = height;
		desc.mFormat = format;
		desc.mUsage = USAGE_IMMUTABLE;
		desc.mBindFlags = BIND_SHADER_RESOURCE;

		SubresourceData resourceData;
		resourceData.mSysMem = data;
		resourceData.mSysMemPitch = width * device.GetFormatStride(format);
		
		return device.CreateTexture2D(&desc, &resourceData, texture);

	}

	void TextureHelper::SwapTexture(Texture2D& texture1, Texture2D& texture2)
	{
	}

}
