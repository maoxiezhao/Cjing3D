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

class ModelImporter
{
public:
	ModelImporter();
	~ModelImporter();

	bool Load(Model& model, const std::string& filePath, const std::string& name = "");

private:
	void ProcessNode(Model& model, const aiScene* aScene, aiNode* aNode );
	void LoadMesh(Model& model, const aiScene* aScene, aiMesh* aMesh);
	void LoadAnimation(Model& model);

};

}