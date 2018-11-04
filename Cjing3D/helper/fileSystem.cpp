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

			// ����app��д�ļ�·����һ��������userdata/Documents·����
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
				// ����app�ļ���
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

		// �򿪶�Ӧ��ԴĿ¼�����ö�Ӧ����Ѱ·����������user dir�µ�д·��
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

			// ���������ĵ�·��
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

		// ����д·��,ͨ����userdata·��������Զ���д·��
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
				// ����Ŀ¼������д·�������searchpath
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
		*	\brief ��ȡ������const char*���أ������ݽ����û�ά��
		*	\param data ����buffer
		*	\return �����ļ��ĳ���
		*/
		char* ReadFileBytes(const string & name, size_t& length)
		{
			// ȷ���ļ�����
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
			// ȷ���ļ�����
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
		*	\brief ��ȡ���·��
		*
		*	����curPath��������·��������curPath�п�ͷ����./
		*	����srcPath��һ��Ŀ¼��Ѱ���ļ�
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