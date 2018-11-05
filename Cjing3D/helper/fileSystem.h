#pragma once

#include <string>

namespace Cjing3D {

	namespace FileData
	{
		static const char* NOT_ASSIGNED = "N/A";
		static const char* DATA_FORMAT_EXTENSION = ".xml";

		bool OpenData(const std::string&dataName, const std::string& dataPath);
		void CloseData();

		// diretories
		bool CreateDirectory(const std::string& path);
		bool DeleteDirectory(const std::string& path);
		bool IsDirectoryExists(const std::string& path);

		// files
		bool IsFileExists(const std::string& name);
		char* ReadFileBytes(const std::string& name, size_t& length);
		std::string ReadFile(const std::string& name);
		bool SaveFile(const std::string& name, const std::string&buffer);
		bool DeleteFile(const std::string& name);

		// string parsing
		std::wstring StringToWString(const std::string& str);

		std::string GetExtensionFromFilePath(const std::string& filePath);
	}
}