#include "resource\modelImporter\modelImporter.h"
#include "core\globalContext.hpp"
#include "resource\resourceManager.h"
#include "system\sceneSystem.h"
#include "renderer\renderer.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "utils\tinyobjloader.h"

#include <filesystem>

namespace Cjing3D
{
namespace ModelImporter 
{
	namespace {
		bool loadVertexColors = false;
	}

	ECS::Entity ImportModelObj(const std::string& fileName, Scene& scene)
	{
		std::filesystem::path path(fileName);

		auto& device = Renderer::GetDevice();
		auto resourceManager = GetGlobalContext().gResourceManager;

		tinyobj::attrib_t objAttrib;
		std::vector <tinyobj::shape_t> objShapes;
		std::vector<tinyobj::material_t> objMaterials;
		std::string obj_errors;

		Logger::Info("Load obj model, model name:" + path.string() + " model path:" + path.parent_path().string());
		std::string parentPath = std::string(path.parent_path().string().c_str()) + "/";
		bool success = tinyobj::LoadObjCjing3D(&objAttrib, &objShapes, &objMaterials, &obj_errors, path.string().c_str(), parentPath.c_str(), true);
		if (success == false) {
			Debug::Warning("Failed to open model'" + path.string() + "', " + obj_errors);
			return ECS::INVALID_ENTITY;
		}

		Scene newScene;

		// load material
		std::vector<ECS::Entity> materialArray;
		for (auto& objMaterial : objMaterials)
		{
			ECS::Entity materialEntity = newScene.CreateEntityMaterial(objMaterial.name);
			MaterialComponent* material = newScene.GetComponent<MaterialComponent>(materialEntity);

			material->mBaseColor = { objMaterial.diffuse[0], objMaterial.diffuse[1], objMaterial.diffuse[2], objMaterial.diffuse[3] };
			material->mRoughness = objMaterial.roughness;
			material->mMetalness = objMaterial.metallic;

			// 记录名字是为了以后material的序列化
			material->mBaseColorMapName = objMaterial.diffuse_texname;
			material->mNormalMapName = objMaterial.normal_texname;
			material->mSurfaceMapName = objMaterial.specular_texname;

			if (material->mNormalMapName.empty()) {
				material->mNormalMapName = objMaterial.bump_texname;
			}

			if (material->mBaseColorMapName.empty() == false)
			{
				auto texPath = FileData::ConvertToAvailablePath(parentPath + material->mBaseColorMapName);
				material->mBaseColorMap = resourceManager->GetOrCreate<TextureResource>(StringID(texPath));
			}
			if (material->mNormalMapName.empty() == false)
			{
				auto texPath = FileData::ConvertToAvailablePath(parentPath + material->mNormalMapName);
				material->mNormalMap = resourceManager->GetOrCreate<TextureResource>(StringID(texPath));
			}
			if (material->mSurfaceMapName.empty() == false)
			{
				auto texPath = FileData::ConvertToAvailablePath(parentPath + material->mSurfaceMapName);
				material->mSurfaceMap = resourceManager->GetOrCreate<TextureResource>(StringID(texPath));
			}

			materialArray.push_back(materialEntity);
		}

		if (objMaterials.empty())
		{
			ECS::Entity materialEntity = newScene.CreateEntityMaterial("ObjDefaultMaterial");
			materialArray.push_back(materialEntity);
		}

		Entity rootEntity = INVALID_ENTITY;			
		if (objShapes.size() > 1)
		{
			rootEntity = ECS::CreateEntity();
			TransformComponent& rootTransform = newScene.GetOrCreateTransformByEntity(rootEntity);
		}

		// load shape
		for (auto& objShape : objShapes)
		{
			const auto shapeName = objShape.name;
			ECS::Entity objectEntity = newScene.CreateEntityObject(shapeName);
			ECS::Entity meshEntity = newScene.CreateEntityMesh(shapeName + "_mesh");

			ObjectComponent* object = newScene.GetComponent<ObjectComponent>(objectEntity);
			MeshComponent* mesh = newScene.GetComponent<MeshComponent>(meshEntity);

			object->mMeshID = meshEntity;

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

					Color4 color = Color4::White();
					if (loadVertexColors && objAttrib.colors.empty() == false)
					{
						color.SetFloatR(objAttrib.colors[index.vertex_index * 3 + 0]);
						color.SetFloatG(objAttrib.colors[index.vertex_index * 3 + 1]);
						color.SetFloatB(objAttrib.colors[index.vertex_index * 3 + 2]);
					}

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
							objAttrib.texcoords[index.texcoord_index * 2 + 0],
							1 - objAttrib.texcoords[index.texcoord_index * 2 + 1]
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

					static const bool transformToLH = true;
					if (transformToLH) 
					{
						pos[2] *= -1;
						normal[2] *= -1;
					}

					U32 hashValue = 0;
					HashCombine(hashValue, index.vertex_index);
					HashCombine(hashValue, index.normal_index);
					HashCombine(hashValue, index.texcoord_index);
					HashCombine(hashValue, materialIndex);

					if (verticesSet.count(hashValue) == 0)
					{
						verticesSet[hashValue] = (U32)mesh->mVertexPositions.size();
						mesh->mVertexPositions.push_back(pos);
						mesh->mVertexNormals.push_back(normal);
						mesh->mVertexTexcoords.push_back(tex);

						if (loadVertexColors) {
							mesh->mVertexColors.push_back(color.mRGBA);
						}
					}

					mesh->mIndices.push_back(verticesSet[hashValue]);
					mesh->mSubsets.back().mIndexCount++;
				}
			}

			mesh->SetupRenderData(device);

			// attach to root entity
			if (rootEntity != INVALID_ENTITY) {
				newScene.AttachEntity(objectEntity, rootEntity, true);
			}
			else {
				rootEntity = objectEntity;
			}
		}

		scene.Merge(newScene);

		return rootEntity;
	}
}
}