#include "modelImporter.h"
#include "assimp\Importer.hpp"
#include "assimp\postprocess.h"
#include "assimp\ProgressHandler.hpp"
#include "assimp\version.h"
#include "assimp\scene.h"

#include "world\component\renderable.h"
#include "world\actor.h"

#include <unordered_map>

using namespace Assimp;

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
	void ExtractVerticeIndices(aiMesh& aMesh, Mesh& mesh)
	{
		auto& vertices = mesh.GetVertexPosNormalTex();
		auto& indices  = mesh.GetIndiecs();

		std::unordered_map<size_t, U32> vertexMapping = {};

		for(int faceIndex = 0; faceIndex < aMesh.mNumFaces; faceIndex++)
		{
			aiFace face = aMesh.mFaces[faceIndex];
			if (face.mNumIndices < 3) {
				continue;
			}

			for (int index = 0; index < face.mNumIndices; index+= 3)
			{
				U32 reordered_indices[] = {
					face.mIndices[index + 0],
					face.mIndices[index + 1],
					face.mIndices[index + 2]
				};

				// 获取顶点数据
				for (auto& index : reordered_indices)
				{
					Mesh::VertexPosNormalTex vertex;

					// Position
					auto aPos = aMesh.mVertices[index];
					vertex.pos = { aPos.x, aPos.y, aPos.z };
					// Normal
					if (aMesh.mNormals != nullptr)
					{
						auto aNormal = aMesh.mNormals[index];
						vertex.normal = { aNormal.x, aNormal.y, aNormal.z };
					}
					// TexCoords
					if (aMesh.mTextureCoords != nullptr)
					{
						vertex.tex = { 
							aMesh.mTextureCoords[0][index].x, 
							aMesh.mTextureCoords[0][index].y
						};
					}

					// 通过HashMap将全局Mesh数据转换到局部数据
					size_t vertexHash = std::hash<int>{}(index);
					if (vertexMapping.count(vertexHash) == 0)
					{
						vertexMapping[vertexHash] = vertices.size();
						vertices.push_back(vertex);
					}
					indices.push_back(vertexMapping[vertexHash]);
				}
			}
		}
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
		//ProcessNode(model, scene, scene->mRootNode);

		// load animation
		LoadAnimation(model);

		importer.FreeScene();

		result = true;
	}

	return result;
}

void ModelImporter::ProcessNode(Model& model, const aiScene * aScene, aiNode * aNode, Actor* parentNode, Actor* childNode)
{
	// if is root node
	if (aNode->mParent == nullptr)
	{

	}

	for (int i = 0; i < aNode->mNumMeshes; i++)
	{
		aiMesh* mesh = aScene->mMeshes[aNode->mMeshes[i]];
		if (mesh == nullptr) {
			continue;
		}

		//LoadMesh(model, aScene, mesh);
	}

	for (int i = 0; i < aNode->mNumChildren; i++)
	{
		//ProcessNode(model, aScene, aNode->mChildren[i]);
	}

}

void ModelImporter::LoadMesh(Model& model, const aiScene * aScene, aiMesh * aMesh)
{
	const auto name = aMesh->mName;
	MeshPtr meshPtr = std::make_shared<Mesh>(name.C_Str());
	ExtractVerticeIndices(*aMesh, *meshPtr);

	model.AddMesh(name.C_Str(), meshPtr);
}

void ModelImporter::LoadAnimation(Model & model)
{
}

}
