#pragma once

#include "common\common.h"
#include "renderer\components\model.h"

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
	void ReadNodeHierarchy(const aiScene* aScene, aiNode* aNode, Model* model);
	void LoadMesh(const aiScene* aScene, aiMesh* aMesh, Model* model);


};

}