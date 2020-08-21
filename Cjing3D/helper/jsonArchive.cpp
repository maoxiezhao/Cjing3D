#include "jsonArchive.h"
#include "helper\fileSystem.h"

namespace Cjing3D
{
	const U32 JsonArchive::currentArchiveVersion = 1;

	JsonArchive::JsonArchive(const std::string& path, ArchiveMode mode) :
		ArchiveBase(path, mode)
	{
		OnLoad();
	}

	JsonArchive::~JsonArchive()
	{
		OnUnload();
	}

	void JsonArchive::OnLoad()
	{
		if (mMode == ArchiveMode::ArchiveMode_Read)
		{
			if (!Load(mFilePath)) {
				return;
			}
			try	
			{
				mRootJson = nlohmann::json::parse(mDataBuffer, mDataBuffer + mDataSize);
			}
			catch (const std::exception& e) 
			{
				Debug::Warning("Filed to open json file:" + mFilePath + " ," + e.what());
				Close();
				return;
			}
		}
	}

	void JsonArchive::OnUnload()
	{
		while (!mJsonStack.empty()) {
			mJsonStack.pop();
		}

		if (mMode == ArchiveMode::ArchiveMode_Write) {
			Save(mFilePath);
		}
	}

	bool JsonArchive::Save(const std::string& path)
	{
		std::string string = mRootJson.dump(0);
		if (string.empty()) {
			return false;
		}

		return FileData::SaveFile(path, string);
	}

	nlohmann::json* JsonArchive::GetCurrentJson()
	{
		if (mJsonStack.empty()) {
			return &mRootJson;
		}
		return mJsonStack.top();
	}

	const nlohmann::json* JsonArchive::GetCurrentJson() const
	{
		if (mJsonStack.empty()) {
			return &mRootJson;
		}
		return mJsonStack.top();
	}

	size_t JsonArchive::GetCurrentValueCount() const
	{
		return GetCurrentJson()->size();
	}
}