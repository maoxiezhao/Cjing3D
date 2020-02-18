#include "archive.h"
#include "helper\fileSystem.h"

namespace Cjing3D
{
	U32 Archive::currentArchiveVersion = 1;

	const U32 defaultDataBufferSize = 128;

	Archive::Archive(const std::string& path, ArchiveMode mode) :
		mFilePath(path),
		mMode(mode)
	{
		if (mode == ArchiveMode::ArchiveMode_Read) 
		{
			if (!FileData::IsFileExists(path)) {
				return;
			}

			size_t dataSize = 0;
			mDataBuffer = FileData::ReadFileBytes(path, dataSize);
			mDataSize = dataSize;

			this->operator>>(mCurrentArchiveVersion);
		}
		else
		{
			// �����дģʽ��������д��汾��
			mCurrentArchiveVersion = Archive::currentArchiveVersion;
			mDataSize = 128;
			mDataBuffer = new char[mDataSize];

			this->operator<<(mCurrentArchiveVersion);
		}
	}

	Archive::~Archive()
	{
		Close();
	}

	bool Archive::IsOpen() const
	{
		return mDataBuffer != nullptr;
	}

	void Archive::Close()
	{
		if (mMode == ArchiveMode::ArchiveMode_Write) {
			Save(mFilePath);
		}

		SAFE_DELETE_ARRAY(mDataBuffer);
	}

	void Archive::Save(const std::string& path)
	{
		if (mFilePath.empty()) {
			return;
		}

		FileData::SaveFile(mFilePath, mDataBuffer, static_cast<size_t> (mDataSize));
	}

	std::string Archive::GetDirectory() const
	{
		return FileData::GetParentPath(mFilePath) + "/";
	}

	void Archive::WriteImpl(const void* data, U32 size)
	{
		// ����µĴ�С���ڷ����С�������·���2���Ĵ�С
		U32 newSize = mReadPos + size;
		if (newSize > mDataSize) 
		{
			U32 newBufferSize = newSize * 2;
			char* newBuffer = new char[newBufferSize];
			memcpy_s(newBuffer, newBufferSize, mDataBuffer, mDataSize);
			mDataSize = newBufferSize;

			SAFE_DELETE_ARRAY(mDataBuffer);
			mDataBuffer = newBuffer;
		}

		memcpy_s(reinterpret_cast<void*>(mDataBuffer + mReadPos), mDataSize, data, size);
		mReadPos = newSize;
	}

	void Archive::ReadImpl(void* data, U32 size)
	{
		memcpy_s(data, size, reinterpret_cast<void*>(mDataBuffer + mReadPos), size);
		mReadPos += size;
	}
}