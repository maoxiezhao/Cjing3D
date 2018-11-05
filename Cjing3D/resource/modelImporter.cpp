#include "modelImporter.h"
#include "Assimp_4.1.0\assimp\Importer.hpp"
#include "Assimp_4.1.0\assimp\postprocess.h"
#include "Assimp_4.1.0\assimp\ProgressHandler.hpp"
#include "Assimp_4.1.0\assimp\version.h"

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


		result = true;
	}

	return result;
}

void ModelImporter::ReadNodeHierarchy(const aiScene * aScene, aiNode * aNode, Model * model)
{
}

void ModelImporter::LoadMesh(const aiScene * aScene, aiMesh * aMesh, Model * model)
{
}

}
