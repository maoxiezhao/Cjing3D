#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\components\mesh.h"
#include "renderer\components\material.h"
#include "renderer\components\component.h"

#include <map>

namespace Cjing3D
{

using MeshCollection = std::map<std::string, MeshPtr>;
using materialCollection = std::map<std::string, MaterialPtr>;

class Model
{
public:
	Model();
	~Model();

	bool LoadFromFile(const std::string filePath);
	bool SaveToFile(const std::string& filePath);

	void Update();

private:
	bool LoadFromEngineFormat(const std::string& filePath);
	bool LoadFromExternalFormat(const std::string& filePath);
	
	MeshCollection mMeshCollection;
	materialCollection mMaterialCollection;
	
};

}