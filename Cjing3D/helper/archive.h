#pragma once

#include "common\definitions.h"

#include <string>
#include <vector>

namespace Cjing3D {

	enum class ArchiveMode
	{
		ArchiveMode_Read,
		ArchiveMode_Write
	};

	class ArchiveBase
	{
	public:
		ArchiveBase(const std::string& path, ArchiveMode mode);
		~ArchiveBase();

		bool IsOpen()const;
		void Close();
		std::string GetDirectory()const;

		virtual bool Load(const std::string& path);
		virtual bool Save(const std::string& path);

	protected:
		ArchiveMode mMode = ArchiveMode::ArchiveMode_Read;
		std::string mFilePath;

		char* mDataBuffer = nullptr;
		U32 mDataSize = 0;
		U32 mReadPos = 0;
		U32 mCurrentArchiveVersion = 0;
	};
}