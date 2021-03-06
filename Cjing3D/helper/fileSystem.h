#pragma once

#include "common\common.h"

#include <filesystem>
#include <string>
#include <vector>

namespace Cjing3D {

	namespace FileData
	{
		static const char* NOT_ASSIGNED = "N/A";
		static const char* DATA_FORMAT_EXTENSION = ".xml";

		bool OpenData(const std::string& programName, const std::string& assetPath, const std::string& assetName);
		bool IsDataOpened();
		void CloseData();
		void Reset();

		// diretories
		bool CreateDirectory(const std::string& path);
		bool DeleteDirectory(const std::string& path);
		bool IsDirectoryExists(const std::string& path);

		// files
		bool IsFileExists(const std::string& name);
		char* ReadFileBytes(const std::string& name, size_t& length);
		bool ReadFileBytes(const std::string& name, std::vector<unsigned char>& data);
		std::string ReadFile(const std::string& name);
		bool SaveFile(const std::string& name, const std::string&buffer);
		bool SaveFile(const std::string& name, const char* buffer, size_t length);
		bool DeleteFile(const std::string& name);

		// path
		std::string GetExtensionFromFilePath(const std::string& filePath);
		std::string GetParentPath(const std::string& filePath);
		bool IsAbsolutePath(const std::string& path);
		std::string ConvertToAvailablePath(const std::string& path);
		std::string CombinePath(const std::string& path1, const std::string& path2);
	}
}