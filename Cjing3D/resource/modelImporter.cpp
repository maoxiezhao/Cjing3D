#include "modelImporter.h"
#include "Assimp_4.1.0\assimp\Importer.hpp"
#include "Assimp_4.1.0\assimp\postprocess.h"
#include "Assimp_4.1.0\assimp\ProgressHandler.hpp"
#include "Assimp_4.1.0\assimp\version.h"

#include "world\component\renderable.h"
#include "world\actor.h"

namespace Cjing3D
{

// TODO: 将Assimp Loader剥离

namespace {

	// 自定义的进度处理器，用于获取当前加载进度
	class CustomProgressHandle : public Assimp::ProgressHandler
	{
	public:
		bool Update(float percentage) { return true; }
	};

	// 读取模型设置
	static unsigned int importFlag = 
		aiProcess_CalcTangentSpace |
		aiProcess_GenSmoothNormals |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ImproveCacheLocality |
		aiProcess_LimitBoneWeights |
		aiProcess_SplitLargeMeshes |
		aiProcess_Triangulate |
		aiProcess_GenUVCoords |
		aiProcess_SortByPType |
		aiProcess_FindDegenerates |
		aiProcess_FindInvalidData |
		aiProcess_FindInstances |
		aiProcess_ValidateDataStructure |
		aiProcess_OptimizeMeshes |
		aiProcess_Debone |
		aiProcess_ConvertToLeftHanded;

	// extract vertice
	void ExtractVertice(aMesh& mesh, Model& model, int& vertexOffset)
	{

	}

	// extract indices
	void ExtractIndices(aMesh& mesh, Model& model, int& indicesOffset)
	{

	}

}


ModelImporter::ModelImporter()
{
}

ModelImporter::~ModelImporter()
{
}

bool ModelImporter::Load(Model & model, const std::string & filePath, const std::string & name)
{
	bool result = false;

	Assimp::Importer importer;
	importer.SetPropertyInteger(AI_CONFIG_PP_ICL_PTCACHE_SIZE, 64);					
	importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, aiComponent_CAMERAS | aiComponent_LIGHTS);
	importer.SetProgressHandler(new CustomProgressHandle());

	const aiScene* scene = importer.ReadFile(filePath, importFlag);
	if (scene != nullptr)
	{
		// process node
		ProcessNode(model, scene, scene->mRootNode);

		// load animation
		LoadAnimation(model);

		importer.FreeScene();

		result = true;
	}

	return result;
}

void ModelImporter::ProcessNode(Model& model, const aiScene * aScene, aiNode * aNode)
{
	// if is root node
	if (aNode->mParent == nullptr)
	{

	}

	for (int i = 0; i < aNode->mNumMeshed; i++)
	{
		aiMesh* mesh = aScene->mMeshes[aNode->mMeshes[i]];
		if (mesh == nullptr) {
			continue;
		}

		LoadMesh(model, aScene, mesh);
	}

	for (int i = 0; i < aNode->mNumChildren; i++)
	{
		ProcessNode(model, aScene, aNode->mChildren[i]);
	}

}

void ModelImporter::LoadMesh(Model& model, const aiScene * aScene, aiMesh * aMesh)
{
	int vertexoffset = 0;
	int indicesOffset = 0;

	ExtractVertice(*aMesh, model, vertexoffset);
	ExtractIndices(*aMesh, model, indicesOffset);
}

}
