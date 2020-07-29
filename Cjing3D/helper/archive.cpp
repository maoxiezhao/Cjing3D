#include "archive.h"
#include "helper\fileSystem.h"

namespace Cjing3D
{
	ArchiveBase::ArchiveBase(const std::string& path, ArchiveMode mode) :
		mFilePath(path),
		mMode(mode)
	{
	}

	ArchiveBase::~ArchiveBase()
	{
		Close();
	}

	bool ArchiveBase::IsOpen() const
	{
		return mDataBuffer != nullptr;
	}

	void ArchiveBase::Close()
	{
		SAFE_DELETE_ARRAY(mDataBuffer);
	}

	bool ArchiveBase::Load(const std::string& path)
	{
		if (!FileData::IsFileExists(mFilePath)) {
			return false;
		}

		size_t dataSize = 0;
		mDataBuffer = FileData::ReadFileBytes(mFilePath, dataSize);
		if (mDataBuffer == nullptr) {
			return false;
		}

		mDataSize = dataSize;
		return true;
	}

	bool ArchiveBase::Save(const std::string& path)
	{
		if (mFilePath.empty()) {
			return false;
		}

		return FileData::SaveFile(mFilePath, mDataBuffer, static_cast<size_t> (mDataSize));
	}

	std::string ArchiveBase::GetDirectory() const
	{
		return FileData::GetParentPath(mFilePath) + "/";
	}
}