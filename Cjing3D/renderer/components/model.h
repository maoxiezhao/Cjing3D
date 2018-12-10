#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\components\mesh.h"
#include "renderer\components\material.h"
#include "resource\resource.h"

#include <map>

namespace Cjing3D
{

class ResourceManager;

using MeshCollection = std::map<std::string, MeshPtr>;
using MaterialCollection = std::map<StringID, MaterialPtr>;

class Model : public Resource
{
public:
	Model();
	~Model();

	void LoadFromFile(const std::string filePath, ResourceManager& resourceManager);

	void AddMaterial(const std::string& name, MaterialPtr material);
	void SetMesh(MeshPtr mesh);

private:
	void LoadFromEngineFormat(const std::string& filePath, ResourceManager& resourceManager);
	void LoadFromExternalFormat(const std::string& filePath, ResourceManager& resourceManager);
	
private:
	MeshPtr mMesh;		
	MaterialCollection mMaterialCollection;
	
};

}