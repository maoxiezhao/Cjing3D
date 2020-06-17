#include "audioDevice.h"
#include "helper\fileSystem.h"

namespace Cjing3D
{
namespace Audio
{
	bool AudioDevice::LoadSound(const std::string& name, SoundResource& resource)
	{
		if (!FileData::IsFileExists(name))
		{
			Debug::Warning("Failed to load sound:" + name);
			return false;
		}

		size_t length = 0;
		const char* data = FileData::ReadFileBytes(name, length);
		if (data == nullptr)
		{
			Debug::Warning("Failed to load sound:" + name);
			return false;
		}

		return LoadSoundImpl(data, length, resource);
	}

	bool AudioDevice::LoadSound(const char* data, size_t length, SoundResource& resource)
	{
		return LoadSoundImpl(data, length, resource);
	}
}
}
