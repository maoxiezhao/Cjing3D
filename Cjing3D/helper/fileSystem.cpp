#include "fileSystem.h"
#include "debug.h"
#include "physfs\physfs.h"

#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <filesystem>

using std::string;

// 考虑使用std::filesystem来构建engine filesystem

namespace Cjing3D {
	namespace FileData {
		namespace
		{
			string dataName_ = "";
			string dataPath_ = "";
			string dataWriteDir_ = "";
			string appWriteDir_ = "Cjing3D";
			string buildWriteDir_ = "build/generating";

			// 设置app的写文件路径，一般设置在userdata/Documents路径下
			void SetAppWriteDir(const string& appWriteDir)
			{
				Debug::CheckAssertion(!appWriteDir.empty(),
					"The app write path is empty.");

				appWriteDir_ = "Documents/" + appWriteDir;
				if (!PHYSFS_setWriteDir(PHYSFS_getUserDir()))
				{
					Debug::CheckAssertion(!appWriteDir.empty(),
						"Can not set write dir in user dir.");
				}
				// 创建app文件夹
				PHYSFS_mkdir(appWriteDir_.c_str());
				string fullAppWriteDir = string(PHYSFS_getUserDir()) + "/" + appWriteDir_;
				if (!PHYSFS_setWriteDir(fullAppWriteDir.c_str()))
				{
					Debug::CheckAssertion(!appWriteDir.empty(),
						"Can not set write dir in user dir.");
				}

			}

			bool IsOpen()
			{
				return !dataPath_.empty();
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

			char* ReadFileBytesByPhysfs(const string& name, size_t& length)
			{
				// 确保文件存在
				Debug::CheckAssertion(PHYSFS_exists(name.c_str()),
					string("the file:") + name + " isn't exits.");

				PHYSFS_file* file = PHYSFS_openRead(name.c_str());
				Debug::CheckAssertion(file != nullptr,
					string("the file:") + name + " loaded failed.");

				size_t size = static_cast<size_t>(PHYSFS_fileLength(file));
				char* buffer = new char[size];
				length = size;

				PHYSFS_read(file, buffer, 1, (PHYSFS_uint32)size);
				PHYSFS_close(file);

				return buffer;
			}

			bool SaveFileByPhysfs(const std::string& name, const char* buffer, size_t length)
			{
				PHYSFS_File* file = PHYSFS_openWrite(name.c_str());
				if (file == nullptr)
					Debug::Die(string("the file:") + name + " created failed.");

				if (!PHYSFS_write(file, buffer, (PHYSFS_uint32)length, 1))
					Debug::Die(string("the file:") + name + "writed failed.");

				PHYSFS_close(file);
				return true;
			}
		}

		// 打开对应资源目录，设置对应的搜寻路径，设置在user dir下的写路径
		bool OpenData(const string&dataName, const string& dataPath)
		{
			if (IsOpen())
				CloseData();

			if (dataName.empty())
				PHYSFS_init(nullptr);
			else
				PHYSFS_init(dataName.c_str());

			dataName_ = dataName;
			dataPath_ = dataPath;

			string dirDataPath = dataPath;
			const string&baseDir = PHYSFS_getBaseDir();
			PHYSFS_addToSearchPath(dirDataPath.c_str(), 1);
			PHYSFS_addToSearchPath((baseDir + "\\" + dirDataPath).c_str(), 1);

			PHYSFS_permitSymbolicLinks(1);

			// 设置数据文档路径
			//SetAppWriteDir(DEFAULT_APP_WRITE_DIR);

			return true;
		}

		void CloseData()
		{
			if (!IsOpen())
				return;

			dataName_.clear();
			dataPath_.clear();
			bool result = PHYSFS_deinit();
			return;
		}

		void Reset()
		{
			for (char** i = PHYSFS_getSearchPath(); *i != NULL; i++)
				printf("[%s] is in the search path.\n", *i);

			std::string dataName = dataName_;
			std::string dataPah = dataPath_;

			CloseData();
			OpenData(dataName, dataPah);
		}

		// 设置写路径,通过对userdata路径后添加自定义写路径
		void SetGeneratingWriteDir(const std::string & writeDir)
		{
			if (!dataWriteDir_.empty()) {
				PHYSFS_removeFromSearchPath(PHYSFS_getWriteDir());
			}

			dataWriteDir_ = writeDir;

			if (!PHYSFS_setWriteDir(writeDir.c_str()))
			{
				Debug::Die("Failed to set Write dir:" + dataWriteDir_ + " Error:" + PHYSFS_getLastError());
			}
			if (!dataWriteDir_.empty())
			{
				// 创建目录，设置写路径，添加searchpath
				PHYSFS_mkdir(buildWriteDir_.c_str());

				std::string fullWriteDir = dataWriteDir_ + "/" + buildWriteDir_;
				if (!PHYSFS_setWriteDir(fullWriteDir.c_str()))
				{
					Debug::Die("Failed to set Write dir:" + fullWriteDir + " Error:" + PHYSFS_getLastError());
				}
				PHYSFS_addToSearchPath(PHYSFS_getWriteDir(), 0);
			}

		}

		std::vector<std::string> EnumerateFiles(const std::string & path)
		{
			std::vector<std::string> result;
			char **rc = PHYSFS_enumerateFiles(path.c_str());

			for (char **i = rc; *i != NULL; i++)
				result.push_back(*i);

			PHYSFS_freeList(rc);

			return result;
		}

		bool IsDirectory(const std::string & path)
		{
			return PHYSFS_isDirectory(path.c_str());
		}

		bool IsFileExists(const string& name)
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
		char* ReadFileBytes(const string & name, size_t& length)
		{
			if (IsAbsolutePath(name)) {
				return ReadFileBytesBySystemIO(name, length);
			}
			else {
				return ReadFileBytesByPhysfs(name, length);
			}
		}

		string ReadFile(const string& name)
		{
			if (IsAbsolutePath(name)) {
				size_t length = 0;
				char* buffer = ReadFileBytesBySystemIO(name, length);
				return string(buffer, length);
			}
			else {
				size_t length = 0;
				char* buffer = ReadFileBytesByPhysfs(name, length);
				return string(buffer, length);
			}
		}

		bool SaveFile(const string& name, const string&buffer)
		{
			return SaveFile(name, buffer.data(), buffer.length());
		}

		bool SaveFile(const std::string& name, const char* buffer, size_t length)
		{
			if (IsAbsolutePath(name)) {
				return SaveFileBySystemIO(name, buffer, length);
			}else {
				return SaveFileByPhysfs(name, buffer, length);
			}
		}

		bool DeleteFile(const string& name)
		{
			if (PHYSFS_delete(name.c_str()))
				return false;
			return true;
		}

		std::string GetExtensionFromFilePath(const std::string & filePath)
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

		/**
		*	\brief 获取相对路劲
		*
		*	根据curPath会计算相对路劲，其中curPath中开头包含./
		*	则会从srcPath上一级目录下寻找文件
		*/
		string GetPositivePath(const string & srcPath, const string & curPath)
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