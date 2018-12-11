#include "model.h"
#include "helper\fileSystem.h"
#include "resource\resourceManager.h"

namespace Cjing3D
{
	Model::Model():
		Resource(Resource_Model),
		mMesh(nullptr)
	{
	}

	Model::~Model()
	{
		mMaterialCollection.clear();
	}

	void Model::LoadFromFile(const std::string filePath, ResourceManager& resourceManager)
	{
		if (FileData::IsFileExists(filePath) == false) {
			Debug::Warning("The file is not exists:" + filePath);
		}

		// use StringID??
		const auto fileFormat = FileData::GetExtensionFromFilePath(filePath);
		if (fileFormat == FileData::DATA_FORMAT_EXTENSION) 
		{
			LoadFromEngineFormat(filePath, resourceManager);
		}
		else
		{
			LoadFromExternalFormat(filePath, resourceManager);
		}

		Logger::Info("Loaded Model Succeed:" + filePath);
	}

	void Model::AddMaterial(const std::string & name, MaterialPtr material)
	{
		if (mMaterialCollection.find(name) != mMaterialCollection.end()) {
			mMaterialCollection.insert(std::make_pair(name, material));
		}
	}

	void Model::SetMesh(MeshPtr mesh)
	{
		mMesh = mesh;
	}

	void Model::LoadFromEngineFormat(const std::string & filePath, ResourceManager& resourceManager)
	{
		// do nothing now...
	}

	void Model::LoadFromExternalFormat(const std::string & filePath, ResourceManager& resourceManager)
	{
		auto& modelImporter = resourceManager.GetModelImporter();
		if (modelImporter.Load(*this, filePath) == true)
		{
			
		}
	}
}