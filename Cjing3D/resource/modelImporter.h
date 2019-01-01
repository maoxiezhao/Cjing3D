#pragma once

#include "common\common.h"
#include "renderer\components\model.h"

// assimp definition
struct aiNode;
struct aiScene;
struct aiMaterial;
struct aiMesh;

namespace Cjing3D
{
class Actor;
class SystemContext;

class ModelImporter
{
public:
	ModelImporter();
	~ModelImporter();

	bool Load(Model& model, const std::string& filePath, const std::string& name = "");

private:
	void ProcessNode(Model& model, const aiScene* aScene, aiNode* aNode, Actor* parentNode, Actor* childNode);
	void LoadMesh(Model& model, const aiScene* aScene, aiMesh* aMesh);
	void LoadAnimation(Model& model);

};

void ImportModelObj(const std::string& fileName, SystemContext& systemContext);

}