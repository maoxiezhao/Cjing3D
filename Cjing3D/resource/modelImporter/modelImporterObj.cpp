#include "resource\modelImporter.h"
#include "core\systemContext.hpp"
#include "resource\resourceManager.h"
#include "world\world.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "utils\tinyObjLoader.h"

#include <filesystem>

namespace Cjing3D
{
	void ImportModelObj(const std::string& fileName, SystemContext& systemContext)
	{
		std::filesystem::path path(fileName);

		auto& renderer = systemContext.GetSubSystem<Renderer>();
		auto& device = renderer.GetDevice();
		auto& resourceManager = systemContext.GetSubSystem<ResourceManager>();
		auto& world = systemContext.GetSubSystem<World>();

		tinyobj::attrib_t objAttrib;
		std::vector <tinyobj::shape_t> objShapes;
		std::vector<tinyobj::material_t> objMaterials;
		std::string obj_errors;

		Logger::Info("Load obj model, model name:" + path.string() + " model path:" + path.parent_path().string());
		std::string parentPath = std::string(path.parent_path().string().c_str()) + "\\";
		bool success = tinyobj::LoadObj(&objAttrib, &objShapes, &objMaterials, &obj_errors, path.string().c_str(), parentPath.c_str(), true);
		if (success == false) {
			Debug::Warning("Failed to open model'" + path.string() + "', " + obj_errors);
			return;
		}

		Scene newScene;

		// load material
		std::vector<ECS::Entity> materialArray;
		for (auto& objMaterial : objMaterials)
		{
			ECS::Entity materialEntity = newScene.CreateEntityMaterial(objMaterial.name);
			std::shared_ptr<MaterialComponent> material = newScene.GetComponent<MaterialComponent>(materialEntity);

			material->mBaseColor = {objMaterial.diffuse[0], objMaterial.diffuse[1], objMaterial.diffuse[2], objMaterial.diffuse[3]};
			material->mRoughness = objMaterial.roughness;
			material->mMetalness = objMaterial.metallic;

			// 记录名字是为了以后material的序列化
			material->mBaseColorMapName = objMaterial.diffuse_texname;
			material->mNormalMapName = objMaterial.normal_texname;
			material->mSurfaceMapName = objMaterial.specular_texname;

			if (material->mBaseColorMapName.empty() == false)
			{
				material->mBaseColorMap = resourceManager.GetOrCreate<RhiTexture2D>(StringID(material->mBaseColorMapName), device);
			}
			if (material->mNormalMapName.empty() == false)
			{
				material->mNormalMap = resourceManager.GetOrCreate<RhiTexture2D>(StringID(material->mNormalMapName), device);
			}
			if (material->mSurfaceMapName.empty() == false)
			{
				material->mSurfaceMap = resourceManager.GetOrCreate<RhiTexture2D>(StringID(material->mSurfaceMapName), device);
			}

			materialArray.push_back(materialEntity);
		}

		if (objMaterials.empty())
		{
			ECS::Entity materialEntity = newScene.CreateEntityMaterial("ObjDefaultMaterial");
			materialArray.push_back(materialEntity);
		}

		// load shape
		for (auto& objShape : objShapes)
		{
			const auto shapeName = objShape.name;
			ECS::Entity objectEntity = newScene.CreateEntityObject(shapeName);
			ECS::Entity meshEntity = newScene.CreateEntityMesh(shapeName + "_mesh");

			std::shared_ptr<ObjectComponent> object = newScene.GetComponent<ObjectComponent>(objectEntity);
			std::shared_ptr<MeshComponent> mesh = newScene.GetComponent<MeshComponent>(meshEntity);
		
			std::unordered_map<U32, U32> materialIndicesSet = {};
			std::unordered_map<U32, U32> verticesSet = {};
			for (size_t i = 0; i < objShape.mesh.indices.size(); i += 3)
			{
				tinyobj::index_t indices[] = {
					objShape.mesh.indices[i + 0],
					objShape.mesh.indices[i + 1],
					objShape.mesh.indices[i + 2],
				};
				for (auto& index : indices)
				{
					F32x3 pos = {
						objAttrib.vertices[index.vertex_index * 3 + 0],
						objAttrib.vertices[index.vertex_index * 3 + 1],
						objAttrib.vertices[index.vertex_index * 3 + 2]
					};

					F32x3 normal = { 0.0f, 0.0f, 0.0f };
					if (objAttrib.normals.empty() == false)
					{
						normal = {
							objAttrib.normals[index.normal_index * 3 + 0],
							objAttrib.normals[index.normal_index * 3 + 1],
							objAttrib.normals[index.normal_index * 3 + 2]
						};
					}

					F32x2 tex = { 0.0f, 0.0f };
					if (objAttrib.texcoords.empty() == false)
					{
						tex = {
							objAttrib.texcoords[index.texcoord_index * 3 + 0],
							objAttrib.texcoords[index.texcoord_index * 3 + 1]
						};
					}

					// material作用于triangel, 同时判断是否是使用新的material，
					// 一个subset对应一个material
					int materialIndex = std::max(0, objShape.mesh.material_ids[i / 3]);
					if (materialIndicesSet.count(materialIndex) == 0)
					{
						materialIndicesSet[materialIndex] = mesh->mSubsets.size();
						MeshComponent::MeshSubset subset;

						subset.mMaterialID = materialArray[materialIndex];
						subset.mIndexOffset = mesh->mIndices.size();
						mesh->mSubsets.push_back(subset);
					}

					U32 hashes[] = {
						std::hash<int>{}(index.vertex_index),
						std::hash<int>{}(index.normal_index),
						std::hash<int>{}(index.texcoord_index),
						std::hash<int>{}(materialIndex),
					};
					// a ^ (b << 1) 必定根据a,b的不同有4种结果
					U32 hashValue = (((hashes[0] ^ (hashes[1] << 1) >> 1) ^ (hashes[2] << 1)) >> 1) ^ (hashes[3] << 1);
					if (verticesSet.count(hashValue) == 0)
					{
						verticesSet[hashValue] = (U32)mesh->mVertexPositions.size();
						mesh->mVertexPositions.push_back(pos);
						mesh->mVertexNormals.push_back(normal);
						mesh->mVertexTexcoords.push_back(tex);
					}

					mesh->mIndices.push_back(verticesSet[hashValue]);
					mesh->mSubsets.back().mIndexCount++;
				}
			}

			mesh->SetupRenderData(device);
		}

		world.GetMainScene().Merge(newScene);
	}
}