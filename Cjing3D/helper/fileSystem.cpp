#include "fileSystem.h"
#include "helper\debug.h"
#include "physfs\physfs.h"

#include <vector>

using std::string;

namespace Cjing3D {
	namespace FileData {
		namespace
		{
			string dataPath_ = "";
			string dataWriteDir_ = "";
			string appWriteDir_ = "Cjing3D";

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

			dataPath_ = dataPath;
			string dirDataPath = dataPath;

			const string&baseDir = PHYSFS_getBaseDir();
			PHYSFS_addToSearchPath(dirDataPath.c_str(), 1);
			PHYSFS_addToSearchPath((baseDir + dirDataPath).c_str(), 1);

			// 设置数据文档路径
			//SetAppWriteDir(DEFAULT_APP_WRITE_DIR);

			return true;
		}

		void CloseData()
		{
			if (!IsOpen())
				return;
			dataPath_.clear();
			PHYSFS_deinit();
		}

		// 设置写路径,通过对userdata路径后添加自定义写路径
		void SetDataWriteDir(const string & writeDir)
		{
			if (!dataWriteDir_.empty())
			{
				PHYSFS_removeFromSearchPath(PHYSFS_getWriteDir());
			}
			dataWriteDir_ = writeDir;
			string fullWriteDir = string(PHYSFS_getUserDir()) + "/" + appWriteDir_;
			if (!PHYSFS_setWriteDir(fullWriteDir.c_str()))
			{
				Debug::Die("Failed to set Write dir:" + appWriteDir_);
			}
			if (!dataWriteDir_.empty())
			{
				// 创建目录，设置写路径，添加searchpath
				PHYSFS_mkdir(dataWriteDir_.c_str());
				fullWriteDir = string(PHYSFS_getUserDir()) + "/" + appWriteDir_ + "/" + dataWriteDir_;
				if (!PHYSFS_setWriteDir(fullWriteDir.c_str()))
				{
					Debug::Die("Failed to set Write dir:" + appWriteDir_);
				}
				PHYSFS_addToSearchPath(PHYSFS_getWriteDir(), 0);
			}

		}

		bool IsFileExists(const string& name)
		{
			return PHYSFS_exists(name.c_str());
		}

		/**
		*	\brief 读取数据以const char*返回，该数据交由用户维护
		*	\param data 数据buffer
		*	\return 返回文件的长度
		*/
		char* ReadFileBytes(const string & name, size_t& length)
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


		string ReadFile(const string& name)
		{
			// 确保文件存在
			Debug::CheckAssertion(PHYSFS_exists(name.c_str()),
				string("The file:") + name + " isn't exits.");

			PHYSFS_file* file = PHYSFS_openRead(name.c_str());
			Debug::CheckAssertion(file != nullptr,
				string("The file:") + name + " loaded failed.");

			size_t size = static_cast<size_t>(PHYSFS_fileLength(file));
			std::vector<char> buffer(size);

			PHYSFS_read(file, buffer.data(), 1, (PHYSFS_uint32)size);
			PHYSFS_close(file);

			return string(buffer.data(), size);
		}

		bool SaveFile(const string& name, const string&buffer)
		{
			PHYSFS_File* file = PHYSFS_openWrite(name.c_str());
			if (file == nullptr)
				Debug::Die(string("the file:") + name + " created failed.");

			if (!PHYSFS_write(file, buffer.data(), (PHYSFS_uint32)buffer.size(), 1))
				Debug::Die(string("the file:") + name + "writed failed.");

			PHYSFS_close(file);
			return true;
		}

		bool DeleteFile(const string& name)
		{
			if (PHYSFS_delete(name.c_str()))
				return false;
			return true;
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