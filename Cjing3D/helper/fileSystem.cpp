#include "fileSystem.h"
#include "debug.h"
#include "physfs.h"

#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <filesystem>

namespace Cjing3D {
	namespace FileData {
		namespace
		{
			std::string programName_ = "";
			std::string assetPath_ = "";
			std::string assetName_ = "";
			std::string writeDir_ = "";

			std::string GetBaseWriteDir()
			{
				//return PHYSFS_getPrefDir("", appName.c_str());

				// 这里路径可能只在win下有效
				return std::string(PHYSFS_getUserDir()) + "Documents";
			}

			// 设置app的写文件路径，一般设置在userdata/Documents路径下
			void SetWriteDir(const std::string& writeDir)
			{
				writeDir_ = writeDir;

				const std::string baseWriteDir = GetBaseWriteDir();
				if (!PHYSFS_setWriteDir(baseWriteDir.c_str())) {
					Debug::Error(std::string("Can not set write dir in user dir:") +
						PHYSFS_getLastError());
				}

				PHYSFS_mkdir(writeDir.c_str());

				std::string fullWriteDir = baseWriteDir + "/" + writeDir;
				if (!PHYSFS_setWriteDir(fullWriteDir.c_str()))
				{
					Debug::Error(std::string("Can not set write dir in user dir:") +
						PHYSFS_getLastError());
				}

				PHYSFS_addToSearchPath(fullWriteDir.c_str(), 1);
			}

			bool IsOpen()
			{
				return !assetPath_.empty();
			}

			char* ReadFileBytesBySystemIO(const std::string& name, size_t& length)
			{
				char* ret = nullptr;

				std::ifstream file(name, std::ios::binary | std::ios::ate);
				if (file.is_open())
				{
					length = (size_t)file.tellg();
					file.seekg(0, file.beg);
					ret = new char[length];
					file.read((char*)ret, length);
					file.close();
				}

				return ret;
			}

			bool ReadFileBytesBySystemIO(const std::string& name, std::vector<unsigned char>& data)
			{
				std::ifstream file(name, std::ios::binary | std::ios::ate);
				if (file.is_open())
				{
					U32 length = (size_t)file.tellg();
					file.seekg(0, file.beg);
					data.resize(length);
					file.read(reinterpret_cast<char*>(&data.at(0)), length);
					file.close();

					return true;
				}
				return false;
			}

			bool SaveFileBySystemIO(const std::string& name, const char* buffer, size_t length)
			{
				std::ofstream file(name, std::ios::binary | std::ios::trunc);
				if (file.is_open())
				{
					file.write((char*)buffer, (std::streamsize)length);
					file.close();
					return true;
				}
				return false;
			}

			char* ReadFileBytesByPhysfs(const std::string& name, size_t& length)
			{
				// 确保文件存在
				Debug::CheckAssertion(PHYSFS_exists(name.c_str()),
					std::string("the file:") + name + " isn't exits.");

				PHYSFS_file* file = PHYSFS_openRead(name.c_str());
				Debug::CheckAssertion(file != nullptr,
					std::string("the file:") + name + " loaded failed.");

				size_t size = static_cast<size_t>(PHYSFS_fileLength(file));
				char* buffer = new char[size];
				length = size;

				PHYSFS_read(file, buffer, 1, (PHYSFS_uint32)size);
				PHYSFS_close(file);

				return buffer;
			}

			bool ReadFileBytesByPhysfs(const std::string& name, std::vector<unsigned char>& data)
			{
				// 确保文件存在
				Debug::CheckAssertion(PHYSFS_exists(name.c_str()),
					std::string("the file:") + name + " isn't exits.");

				PHYSFS_file* file = PHYSFS_openRead(name.c_str());
				Debug::CheckAssertion(file != nullptr,
					std::string("the file:") + name + " loaded failed.");

				size_t size = static_cast<size_t>(PHYSFS_fileLength(file));
				data.resize(size);

				PHYSFS_read(file, data.data(), 1, (PHYSFS_uint32)size);
				PHYSFS_close(file);

				return true;
			}

			bool SaveFileByPhysfs(const std::string& name, const char* buffer, size_t length)
			{
				PHYSFS_File* file = PHYSFS_openWrite(name.c_str());
				if (file == nullptr) {
					Debug::Die(std::string("the file:") + name + " created failed.");
				}

				if (!PHYSFS_write(file, buffer, (PHYSFS_uint32)length, 1)) {
					Debug::Die(std::string("the file:") + name + "writed failed.");
				}

				PHYSFS_close(file);
				return true;
			}

			bool CreateDirectoryByPhysfs(const std::string& name)
			{
				if (PHYSFS_isDirectory(name.c_str())) {
					return true;
				}
				return PHYSFS_mkdir(name.c_str()) != 0;
			}

			bool CreateDirectoryBySystemIO(const std::string& name)
			{
				if (std::filesystem::is_directory(name)) {
					return true;
				}

				return std::filesystem::create_directory(name);
			}

			bool DeleteDirectoryByPhysfs(const std::string& name)
			{
				if (!PHYSFS_isDirectory(name.c_str())) {
					return true;
				}
				return PHYSFS_delete(name.c_str()) != 0;
			}

			bool DeleteDirectoryBySystemIO(const std::string& name)
			{
				if (!std::filesystem::is_directory(name)) {
					return true;
				}

				return std::filesystem::remove(name);
			}
		}

		// 打开对应资源目录，设置对应的搜寻路径，设置在user dir下的写路径
		bool OpenData(const std::string& programName, const std::string& assetPath, const std::string& assetName)
		{
			if (IsOpen()) {
				CloseData();
			}

			if (programName.empty()) {
				PHYSFS_init(nullptr);
			}
			else {
				PHYSFS_init(programName.c_str());
			}

			programName_ = programName;
			assetPath_ = assetPath;
			assetName_ = assetName;

			PHYSFS_permitSymbolicLinks(1);

			// add base dir
			if (!PHYSFS_mount(assetPath_.c_str(), nullptr, 1))
			{
				Debug::Error(std::string("Failed to mount archive, path:") + assetPath + ", " + PHYSFS_getLastError());
				return false;
			}

			// add assets dir
			const std::string assetFullPath = assetPath + "/" + assetName;
			if (!PHYSFS_mount(assetFullPath.c_str(), nullptr, 1))
			{
				Debug::Error(std::string("Failed to mount archive, path") + assetFullPath + ", " + PHYSFS_getLastError());
				return false;
			}
			const std::string& baseDir = PHYSFS_getBaseDir();
			if (!PHYSFS_mount((baseDir + "/" + assetFullPath).c_str(), nullptr, 1))
			{
				Debug::Error(std::string("Failed to mount archive, path") + baseDir + "/" + assetFullPath + ", " + PHYSFS_getLastError());
				return false;
			}

			SetWriteDir(CJING3D_WRITE_DIR);

			return true;
		}

		bool IsDataOpened()
		{
			return IsOpen();
		}

		void CloseData()
		{
			if (!IsOpen())
				return;

			programName_.clear();
			assetPath_.clear();
			PHYSFS_deinit();
		}

		void Reset()
		{
			for (char** i = PHYSFS_getSearchPath(); *i != NULL; i++) {
				printf("[%s] is in the search path.\n", *i);
			}

			std::string programName = programName_;
			std::string assetPath = assetPath_;
			std::string assetName = assetName_;

			CloseData();
			OpenData(programName, assetPath, assetName);
		}

		bool CreateDirectory(const std::string& path)
		{
			if (IsAbsolutePath(path)) {
				return CreateDirectoryBySystemIO(path);
			}
			else {
				return CreateDirectoryByPhysfs(path);
			}
		}

		bool DeleteDirectory(const std::string& path)
		{
			if (IsAbsolutePath(path)) {
				return DeleteDirectoryBySystemIO(path);
			}
			else {
				return DeleteDirectoryByPhysfs(path);
			}
		}

		bool IsDirectoryExists(const std::string& path)
		{
			if (IsAbsolutePath(path)) {
				return std::filesystem::is_directory(path);
			}
			else {
				return PHYSFS_isDirectory(path.c_str());
			}
		}

		std::vector<std::string> EnumerateFiles(const std::string& path)
		{
			std::vector<std::string> result;
			char** rc = PHYSFS_enumerateFiles(path.c_str());

			for (char** i = rc; *i != NULL; i++)
				result.push_back(*i);

			PHYSFS_freeList(rc);

			return result;
		}

		bool IsDirectory(const std::string& path)
		{
			return PHYSFS_isDirectory(path.c_str());
		}

		bool IsFileExists(const std::string& name)
		{
			if (IsAbsolutePath(name)) {
				return true;
			}
			else {
				int result = PHYSFS_exists(name.c_str());
				return result != 0;
			}
		}

		/**
		*	\brief 读取数据以const char*返回，该数据交由用户维护
		*	\param data 数据buffer
		*	\return 返回文件的长度
		*/
		char* ReadFileBytes(const std::string& name, size_t& length)
		{
			if (IsAbsolutePath(name)) {
				return ReadFileBytesBySystemIO(name, length);
			}
			else {
				return ReadFileBytesByPhysfs(name, length);
			}
		}

		bool ReadFileBytes(const std::string& name, std::vector<unsigned char>& data)
		{
			if (IsAbsolutePath(name)) {
				return ReadFileBytesBySystemIO(name, data);
			}
			else {
				return ReadFileBytesByPhysfs(name, data);
			}
		}

		std::string ReadFile(const std::string& name)
		{
			if (IsAbsolutePath(name)) {
				size_t length = 0;
				char* buffer = ReadFileBytesBySystemIO(name, length);
				return std::string(buffer, length);
			}
			else {
				size_t length = 0;
				char* buffer = ReadFileBytesByPhysfs(name, length);
				return std::string(buffer, length);
			}
		}

		bool SaveFile(const std::string& name, const std::string& buffer)
		{
			return SaveFile(name, buffer.data(), buffer.length());
		}

		bool SaveFile(const std::string& name, const char* buffer, size_t length)
		{
			if (IsAbsolutePath(name)) {
				return SaveFileBySystemIO(name, buffer, length);
			}
			else {
				return SaveFileByPhysfs(name, buffer, length);
			}
		}

		bool DeleteFile(const std::string& name)
		{
			if (PHYSFS_delete(name.c_str()))
				return false;
			return true;
		}

		std::string GetExtensionFromFilePath(const std::string& filePath)
		{
			if (filePath.empty() || filePath == NOT_ASSIGNED) {
				return NOT_ASSIGNED;
			}

			auto lastIndex = filePath.find_last_of('.');
			if (lastIndex != std::string::npos)
			{
				return filePath.substr(lastIndex, filePath.length());
			}
			return NOT_ASSIGNED;
		}

		std::string GetParentPath(const std::string& filePath)
		{
			std::filesystem::path path(filePath);
			return path.parent_path().generic_string();
		}

		bool IsAbsolutePath(const std::string& path)
		{
			std::filesystem::path availablePath(path);
			return availablePath.is_absolute();
		}

		std::string ConvertToAvailablePath(const std::string& path)
		{
			std::filesystem::path availablePath(path);
			availablePath = availablePath.lexically_normal();

			return availablePath.generic_string();
		}

		std::string CombinePath(const std::string& path1, const std::string& path2)
		{
			if (!IsAbsolutePath(path1))
			{
				return path1 + "/" + path2;
			}
			else
			{
				std::filesystem::path path(path1);
				return path.append(path2).string();
			}
		}

		/**
		*	\brief 获取相对路劲
		*
		*	根据curPath会计算相对路劲，其中curPath中开头包含./
		*	则会从srcPath上一级目录下寻找文件
		*/
		std::string GetPositivePath(const std::string& srcPath, const std::string& curPath)
		{
			std::string path = srcPath;
			std::string filePath = curPath;
			for (const auto& c : curPath)
			{
				if (c != '.')
					break;
				size_t pos = path.rfind("/", path.length() - 2);
				if (pos != std::string::npos)
				{
					filePath = filePath.substr(1);
					path = path.substr(0, pos);
				}
			}
			return path + filePath;
		}
	}
}