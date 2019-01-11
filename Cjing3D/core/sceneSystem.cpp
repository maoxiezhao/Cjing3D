#include "sceneSystem.h"
#include "renderer\RHI\rhiFactory.h"

namespace Cjing3D
{
	using namespace ECS;

	Scene::Scene()
	{
	}

	void Scene::Update()
	{
	}

	void Scene::Merge(Scene & scene)
	{
		mNames.Merge(scene.mNames);
		mMeshes.Merge(scene.mMeshes);
		mMaterials.Merge(scene.mMaterials);

		// TEMP
		//mNameEntityMap.clear();
		//mNames.ForEachComponent([&](Entity enity, std::shared_ptr<StringID> stringID) {
		//	mNameEntityMap[*stringID] = enity;
		//});
	}

	void Scene::Clera()
	{
		mNameEntityMap.clear();
		mNames.Clear();
		mMaterials.Clear();
		mMeshes.Clear();
	}

	ECS::Entity Scene::CreateEntityMaterial(const std::string & name)
	{
		auto entity = CreateEntityByName(name);
		mMaterials.Create(entity);

		return entity;
	}

	ECS::Entity Scene::CreateEntityMesh(const std::string & name)
	{
		auto entity = CreateEntityByName(name);
		mMeshes.Create(entity);

		return entity;
	}

	ECS::Entity Scene::CreateEntityObject(const std::string & name)
	{
		auto entity = CreateEntityByName(name);
		mObjects.Create(entity);

		return entity;
	}

	ECS::Entity Scene::CreateEntityByName(const std::string & name)
	{
		// 目前不支持名字重名，后一个将无法添加
		auto it = mNameEntityMap.find(name);
		if (it != mNameEntityMap.end())
		{
			Debug::Warning("Duplicate entity name:" + name);
			return it->second;
		}

		Entity entity = CreateEntity();
		mNames.Create(entity)->SetString(name);
		mNameEntityMap[StringID(name)] = entity;

		return entity;
	}

	Entity Scene::GetEntityByName(StringID name)
	{
		auto it = mNameEntityMap.find(name);
		if (it != mNameEntityMap.end())
		{
			return it->second;
		}
		return ECS::INVALID_ENTITY;
	}

	void Scene::RemoveEntity(Entity entity)
	{
		mMaterials.Remove(entity);
		mMeshes.Remove(entity);

		if (mNames.Contains(entity))
		{
			auto nameComponent = mNames.GetComponent(entity);
			if (nameComponent != nullptr)
			{
				mNameEntityMap.erase(*nameComponent);
			}
			mNames.Remove(entity);
		}
	}

	void MeshComponent::SetupRenderData(GraphicsDevice& device)
	{
		// setup index buffer
		{
			const auto result = CreateStaticIndexBuffer(device, *mIndexBuffer, mIndices);
			Debug::ThrowIfFailed(result, "Failed to create index buffer:%08x", result);
		}

		F32x3 mMinPos = { FLT_MAX, FLT_MAX, FLT_MAX };
		F32x3 mMaxPos = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

		// setup vertex pos buffer
		// format: pos + normal + subsetindex
		{
			// 记录每个顶点所在的subset的index
			std::vector<U32> subsetIndices(mVertexPositions.size());

			U32 subsetIndex = 0;
			for (auto& subset : mSubsets)
			{
				auto indexOffset = subset.mIndexOffset;
				for (int i = 0; i < subset.mIndexCount; i++)
				{
					U32 vertexIndex = mIndices[indexOffset + i];
					subsetIndices[vertexIndex] = subsetIndex;
				}
				subsetIndex++;
			}

			// 创建VertexPosNormalSubset结构
			std::vector<VertexPosNormalSubset> vertices(mVertexPositions.size());
			for (int i = 0; i < mVertexPositions.size(); i++)
			{
				F32x3 pos = mVertexPositions[i];
				F32x3 normal = mVertexNormals.size() > 0 ? mVertexNormals[i] : F32x3(0.0f,0.0f, 0.0f);
				normal = F32x3Normalize(normal);
				U32 index = subsetIndices[i];

				auto& vertex = vertices[i];
				vertex.Setup(pos, normal, index);

				mMinPos = F32x3Min(mMinPos, pos);
				mMaxPos = F32x3Max(mMaxPos, pos);
			}

			const auto result = CreateBABVertexBuffer(device, *mVertexBufferPos, vertices);
			Debug::ThrowIfFailed(result, "Failed to create vertex buffer:%08x", result);
		}

		mAABB = AABB(XMLoad(mMinPos), XMLoad(mMaxPos));

		// vertex texcoords
		if (mVertexTexcoords.empty() == false)
		{
			const auto result = CreateStaticVertexBuffer(device, *mVertexBufferTex, mVertexTexcoords, VertexTex::format);
			Debug::ThrowIfFailed(result, "Failed to create vertex texcoords buffer:%08x", result);
		}

	}

	void MeshComponent::VertexPosNormalSubset::Setup(F32x3 pos, F32x3 normal, U32 subsetIndex)
	{
		mPos = pos;
		Setup(normal, subsetIndex);
	}

	void MeshComponent::VertexPosNormalSubset::Setup(F32x3 normal, U32 subsetIndex)
	{
		// 一个32位依次存储 subsetindex, normal_z, normal_y, normal_x
		Debug::CheckAssertion(subsetIndex < 256, "Invalid subset index.");

		mNormalSubsetIndex = (subsetIndex << 24);
		mNormalSubsetIndex |= (U32)((normal[0] * 0.5f + 0.5f) * 256) << 0;
		mNormalSubsetIndex |= (U32)((normal[1] * 0.5f + 0.5f) * 256) << 8;
		mNormalSubsetIndex |= (U32)((normal[2] * 0.5f + 0.5f) * 256) << 16;
	}
}