#include "resource\modelImporter.h"
#include "core\systemContext.hpp"
#include "resource\resourceManager.h"
#include "system\sceneSystem.h"

#include "utils\stb_image.h"

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_FS
#include "utils\tinygltf\tiny_gltf.h"

#include <filesystem>

// gltf format details: https://zhuanlan.zhihu.com/p/65264050

namespace tinygltf
{
	bool FileExists(const std::string& abs_filename, void*) 
	{
		return Cjing3D::FileData::IsFileExists(abs_filename);
	}

	std::string ExpandFilePath(const std::string& filepath, void*)
	{
		DWORD len = ExpandEnvironmentStringsA(filepath.c_str(), NULL, 0);
		char* str = new char[len];
		ExpandEnvironmentStringsA(filepath.c_str(), str, len);

		std::string s(str);
		delete[] str;
		return s;
	}

	bool ReadWholeFile(std::vector<unsigned char>* out, std::string* err, const std::string& filepath, void*) 
	{
		return Cjing3D::FileData::ReadFileBytes(filepath, *out);
	}

	bool WriteWholeFile(std::string* err, const std::string& filepath, const std::vector<unsigned char>& contents, void*) 
	{
		return true;
	}

	bool LoadImageData(Image* image, const int image_idx, std::string* err, std::string* warn,
		int req_width, int req_height, const unsigned char* bytes, int size, void*)
	{
		const int reqComp = 4;
		int w, h, comp;
		unsigned char* data = stbi_load_from_memory(bytes, size, &w, &h, &comp, reqComp);
		if (!data) {
			return false;
		}

		image->width = w;
		image->height = h;
		image->component = reqComp;
		image->image.resize(static_cast<size_t>(w * h * reqComp));
		std::copy(data, data + w * h * reqComp, image->image.begin());

		stbi_image_free(data);

		return true;
	}
}

namespace Cjing3D {
namespace ModelImporter {

	struct LoaderInfo
	{
		tinygltf::Model gltfModel;
		Cjing3D::Scene scene;
		std::map<U32, ECS::Entity> nodeEntityMap;
		std::vector<Texture2DPtr> loadedTextures;
	};

	void LoadTextureFromImageData(tinygltf::Image& image, std::vector<Texture2DPtr>& loadedTextures)
	{
		if (image.image.empty()) {
			return;
		}

		ResourceManager& resourceManager = GlobalGetSubSystem<ResourceManager>();
		if (!resourceManager.Contains<Texture2D>(StringID(image.uri)))
		{
			int width = image.width;
			int height = image.height;
			int channelCount = image.component;

			Renderer& renderer = GlobalGetSubSystem<Renderer>();
			GraphicsDevice& device = renderer.GetDevice();

			TextureDesc desc = {};
			desc.mWidth = width;
			desc.mHeight = height;
			desc.mArraySize = 1;
			desc.mFormat = FORMAT_R8G8B8A8_UNORM;
			desc.mBindFlags = BIND_UNORDERED_ACCESS | BIND_SHADER_RESOURCE;
			desc.mMipLevels = static_cast<U32>(std::log2(std::max(width, height)));

			std::vector<SubresourceData> resourceData(desc.mMipLevels);

			// 对于非dds纹理加载，需要自建mipmap
			U32 mipWidth = width;
			for (int mipLevel = 0; mipLevel < desc.mMipLevels; mipLevel++)
			{
				resourceData[mipLevel].mSysMem = image.image.data();
				resourceData[mipLevel].mSysMemPitch = mipWidth * channelCount;

				mipWidth = std::max(1u, mipWidth / 2);
			}

			Texture2DPtr texture = resourceManager.GetOrCreateEmptyResource<Texture2D>(StringID(image.uri));
			if (texture == nullptr)
			{
				Debug::Warning("Resource Manager can not create texture resource:" + image.uri);
				return;
			}

			const auto result = renderer.GetDevice().CreateTexture2D(&desc, resourceData.data(), *texture);
			if (FAILED(result))
			{
				resourceManager.ClearResource<Texture2D>(StringID(image.uri));
				Debug::Warning("Failed to create render target" + image.uri);
				return;
			}

			renderer.GetDevice().SetResourceName(*texture, image.uri);

			// 创建各个subresource的srv和uav
			for (int mipLevel = 0; mipLevel < desc.mMipLevels; mipLevel++)
			{
				renderer.GetDevice().CreateShaderResourceView(*texture, 0, -1, mipLevel, 1);
				renderer.GetDevice().CreateUnordereddAccessView(*texture, mipLevel);
			}

			if (desc.mMipLevels > 1) {
				renderer.AddDeferredTextureMipGen(*texture);
			}

			// add texture ref count, it will clear in the end.
			loadedTextures.push_back(texture);
		}
	}

	void LoadMaterials(LoaderInfo& loaderInfo, const std::string& parentPath)
	{
		auto& resourceManager = GlobalGetSubSystem<ResourceManager>();
		tinygltf::Model& gltfModel = loaderInfo.gltfModel;
		Scene& newScene = loaderInfo.scene;

		// load materials
		for (auto gltfMaterial : gltfModel.materials)
		{
			ECS::Entity materialEntity = newScene.CreateEntityMaterial(gltfMaterial.name);
			std::shared_ptr<MaterialComponent> material = newScene.GetComponent<MaterialComponent>(materialEntity);

			material->mBaseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
			material->mRoughness = 1.0f;
			material->mMetalness = 1.0f;

			// base value iterator
			auto baseColorTextureIt = gltfMaterial.values.find("baseColorTexture");
			auto baseColorFactorIt = gltfMaterial.values.find("baseColorFactor");
			// addition value iterator
			auto normalTextureIt = gltfMaterial.additionalValues.find("normalTexture");

			// process base values
			if (baseColorTextureIt != gltfMaterial.values.end())
			{
				auto& tex = gltfModel.textures[baseColorTextureIt->second.TextureIndex()];
				auto& img = gltfModel.images[tex.source];
				material->mBaseColorMapName = img.uri;

				LoadTextureFromImageData(img, loaderInfo.loadedTextures);
			}
			if (baseColorFactorIt != gltfMaterial.values.end())
			{
				auto color = baseColorFactorIt->second.ColorFactor();
				for (U32 i = 0; i < 4; i++) {
					material->mBaseColor[i] = static_cast<F32>(color[0]);
				}
			}

			// process additional values 
			if (normalTextureIt != gltfMaterial.additionalValues.end())
			{
				auto& tex = gltfModel.textures[normalTextureIt->second.TextureIndex()];
				auto& img = gltfModel.images[tex.source];
				material->mNormalMapName = img.uri;

				LoadTextureFromImageData(img, loaderInfo.loadedTextures);
			}

			// load textures
			if (material->mBaseColorMapName.empty() == false)
			{
				material->mBaseColorMap = resourceManager.GetOrCreate<Texture2D>(StringID(material->mBaseColorMapName));
			}
			if (material->mNormalMapName.empty() == false)
			{
				material->mNormalMap = resourceManager.GetOrCreate<Texture2D>(StringID(material->mNormalMapName));
			}
			if (material->mSurfaceMapName.empty() == false)
			{
				material->mSurfaceMap = resourceManager.GetOrCreate<Texture2D>(StringID(material->mSurfaceMapName));
			}
		}
		if (newScene.mMaterials.GetCount() <= 0)
		{
			newScene.CreateEntityMaterial("defaultMaterial");
		}
	}

	void LoadMeshes(LoaderInfo& loaderInfo)
	{
		auto& renderer = GlobalGetSubSystem<Renderer>();
		auto& device = renderer.GetDevice();

		tinygltf::Model& gltfModel = loaderInfo.gltfModel;
		Scene& newScene = loaderInfo.scene;

		for (auto& gltfMesh : gltfModel.meshes)
		{
			ECS::Entity meshEntity = newScene.CreateEntityMesh(gltfMesh.name);
			std::shared_ptr<MeshComponent> mesh = newScene.GetComponent<MeshComponent>(meshEntity);

			for (auto& primitive : gltfMesh.primitives)
			{
				const tinygltf::Accessor& accessor = gltfModel.accessors[primitive.indices];
				const tinygltf::BufferView& bufferView = gltfModel.bufferViews[accessor.bufferView];
				const tinygltf::Buffer& buffer = gltfModel.buffers[bufferView.buffer];

				// 首先扩充indices的大小，并为每个primitive创建一个meshSubset
				size_t indexCount = accessor.count;
				size_t indexOffset = mesh->mIndices.size();
				mesh->mIndices.resize(indexCount + indexOffset);

				MeshComponent::MeshSubset& meshSubset = mesh->mSubsets.emplace_back();
				meshSubset.mIndexCount = indexCount;
				meshSubset.mIndexOffset = indexOffset;
				meshSubset.mMaterialID = newScene.mMaterials.GetEntityByIndex(std::max(0, primitive.material));

				// 填充indices
				const unsigned char* data = buffer.data.data() + accessor.byteOffset + bufferView.byteOffset;
				int stride = accessor.ByteStride(bufferView);
				size_t vertexOffset = mesh->mVertexPositions.size();
				switch (stride)
				{
				case 1:
					for (int i = 0; i < indexCount; i += 3)
					{
						mesh->mIndices[indexOffset + i + 0] = vertexOffset + data[i + 0];
						mesh->mIndices[indexOffset + i + 1] = vertexOffset + data[i + 1];
						mesh->mIndices[indexOffset + i + 2] = vertexOffset + data[i + 2];
					};
					break;
				case 2:
					for (int i = 0; i < indexCount; i += 3)
					{
						mesh->mIndices[indexOffset + i + 0] = vertexOffset + ((U16*)data)[i + 0];
						mesh->mIndices[indexOffset + i + 1] = vertexOffset + ((U16*)data)[i + 1];
						mesh->mIndices[indexOffset + i + 2] = vertexOffset + ((U16*)data)[i + 2];
					};
					break;
				case 4:
					for (int i = 0; i < indexCount; i += 3)
					{
						mesh->mIndices[indexOffset + i + 0] = vertexOffset + ((U32*)data)[i + 0];
						mesh->mIndices[indexOffset + i + 1] = vertexOffset + ((U32*)data)[i + 1];
						mesh->mIndices[indexOffset + i + 2] = vertexOffset + ((U32*)data)[i + 2];
					};
					break;
				}

				// 填充各类顶点数据
				for (auto& attribute : primitive.attributes)
				{
					const std::string attrType = attribute.first;
					int accessorIndex = attribute.second;

					const tinygltf::Accessor& accessor = gltfModel.accessors[accessorIndex];
					const tinygltf::BufferView& bufferView = gltfModel.bufferViews[accessor.bufferView];
					const tinygltf::Buffer& buffer = gltfModel.buffers[bufferView.buffer];
					const unsigned char* data = buffer.data.data() + accessor.byteOffset + bufferView.byteOffset;
					int stride = accessor.ByteStride(bufferView);

					// base info
					size_t vertexCount = accessor.count;
					if (attrType == "POSITION")
					{
						mesh->mVertexPositions.resize(vertexOffset + vertexCount);
						Debug::CheckAssertion(stride == 12);
						for (int i = 0; i < vertexCount; i++)
						{
							mesh->mVertexPositions[vertexOffset + i] = ((F32x3*)data)[i];
						}
					}
					else if (attrType == "NORMAL")
					{
						mesh->mVertexNormals.resize(vertexOffset + vertexCount);
						Debug::CheckAssertion(stride == 12);
						for (int i = 0; i < vertexCount; i++)
						{
							mesh->mVertexNormals[vertexOffset + i] = ((F32x3*)data)[i];
						}
					}
					else if (attrType == "TEXCOORD_0")
					{
						mesh->mVertexTexcoords.resize(vertexOffset + vertexCount);
						Debug::CheckAssertion(stride == 8);
						for (int i = 0; i < vertexCount; i++)
						{
							mesh->mVertexTexcoords[vertexOffset + i] = ((F32x2*)data)[i];
						}
					}
					// skeletons
					else if (attrType == "JOINTS_0")
					{
					}
					else if (attrType == "WEIGHTS_0")
					{
					}
				}
			}

			mesh->SetupRenderData(device);
		}
	}

	void LoadNode(int nodeIndex, Entity parentEntity, LoaderInfo& loaderInfo)
	{
		if (nodeIndex < 0) {
			return;
		}

		tinygltf::Node& node = loaderInfo.gltfModel.nodes[nodeIndex];
		Scene& scene = loaderInfo.scene;

		// 如果当前node包含网格，则这个节点可能是骨骼节点或者是普通网格节点
		ECS::Entity currentEntity = ECS::INVALID_ENTITY;
		if (node.mesh >= 0)
		{
			if (node.skin >= 0)
			{
				MeshComponent& mesh = *scene.mMeshes.GetComponentByIndex(node.mesh);

				// 如果是骨骼节点，则将当前entity指向armature entity
				// 同时为mesh创建object，并attach在armature下
				currentEntity = scene.mArmatures.GetEntityByIndex(node.skin);
				mesh.mArmature = currentEntity;

				ECS::Entity objectEntity = scene.CreateEntityObject(node.name);
				auto object = scene.GetComponent<ObjectComponent>(objectEntity);
				object->mMeshID = mesh.GetCurrentEntity();
		
			}
			else
			{
				// 如果是网格节点，则直接创建object
				currentEntity = scene.CreateEntityObject(node.name);
				auto object = scene.GetComponent<ObjectComponent>(currentEntity);
				object->mMeshID = scene.mMeshes.GetEntityByIndex(node.mesh);
			}
		}
		else if (node.camera >= 0)
		{
			// do nothing
		}
		else
		{
			currentEntity = scene.CreateEntityTransform(node.name);
		}

		loaderInfo.nodeEntityMap[nodeIndex] = currentEntity;

		// get skin node transform
		auto currentTransform = scene.mTransforms.GetComponent(currentEntity);
		if (currentTransform == nullptr) {
			Debug::Warning("[ModelImporter::LoadNode] Invalid transform");
			return;
		}

		if (!node.scale.empty()) {
			currentTransform->SetScaleLocal(
				XMFLOAT3((F32)node.scale[0], (F32)node.scale[1], (F32)node.scale[2])
			);
		}
		if (!node.rotation.empty()) {
			currentTransform->SetRotationLocal(
				XMFLOAT4((F32)node.rotation[0], (F32)node.rotation[1], (F32)node.rotation[2], (F32)node.rotation[3])
			);
		}
		if (!node.translation.empty()) {
			currentTransform->SetTranslationLocal(
				XMFLOAT3((F32)node.translation[0], (F32)node.translation[1], (F32)node.translation[2])
			);
		}
		if (!node.matrix.empty())
		{
			XMFLOAT4X4 world;
			for (size_t y = 0; y < 4; y++)
			{
				size_t index = y * 4;
				for (size_t x = 0; x < 4; x++)
				{
					world.m[y][x] = (F32)node.matrix[index + x];
				}
			}

			// 设置变换矩阵，并分配到scale,translation,rotation
			currentTransform->SetWorldTransform(world);
			currentTransform->ApplyTransform();
		}

		// 这里设置worldMatrix为identity，因为在attachEntity时，会计算inverseBindMatrix
		// 因为这里读出的变换已经在骨骼空间，所以inverseBindmatrix应该为identity
		currentTransform->SetWorldTransform(IDENTITYMATRIX);

		if (parentEntity != ECS::INVALID_ENTITY)
		{
			scene.AttachEntity(currentEntity, parentEntity);
		}

		// process children
		if (!node.children.empty())
		{
			for (int child : node.children) {
				LoadNode(child, parentEntity, loaderInfo);
			}
		}
	}

	void ImportModelGLTF(const std::string& fileName, SystemContext& systemContext)
	{
		std::filesystem::path path(fileName);

		auto& renderer = systemContext.GetSubSystem<Renderer>();
		auto& device = renderer.GetDevice();

		tinygltf::TinyGLTF loader;
		loader.SetImageLoader(tinygltf::LoadImageData, nullptr);

		// set custom fs callbacks
		tinygltf::FsCallbacks fsCallbacks = {
		  &tinygltf::FileExists,  &tinygltf::ExpandFilePath,
		  &tinygltf::ReadWholeFile, &tinygltf::WriteWholeFile,
		  nullptr
		};
		loader.SetFsCallbacks(fsCallbacks);

		LoaderInfo loaderInfo = {};
		std::string err, warn;
		bool result = loader.LoadASCIIFromFile(&loaderInfo.gltfModel, &err, &warn, fileName);
		if (!result)
		{
			Debug::Warning("Failed to open gltf model:" + fileName + "', " + err);
			return;
		}
		
		tinygltf::Model& gltfModel = loaderInfo.gltfModel;
		Scene& newScene = loaderInfo.scene;

		// load materials
		std::string parentPath = std::string(path.parent_path().string().c_str()) + "/";
		LoadMaterials(loaderInfo, parentPath);

		// load meshes
		LoadMeshes(loaderInfo);

		// load armature
		for (auto gltfSkin : gltfModel.skins)
		{
			Entity entity = newScene.CreateArmature(gltfSkin.name);
			ArmatureComponent& armature = *newScene.mArmatures.GetComponent(entity);

			if (gltfSkin.inverseBindMatrices >= 0)
			{
				const tinygltf::Accessor& accessor = gltfModel.accessors[gltfSkin.inverseBindMatrices];
				const tinygltf::BufferView& bufferView = gltfModel.bufferViews[accessor.bufferView];
				const tinygltf::Buffer& buffer = gltfModel.buffers[bufferView.buffer];

				size_t count = accessor.count;
				armature.mInverseBindMatrices.resize(count);	
				memcpy(armature.mInverseBindMatrices.data(), &buffer.data[accessor.byteOffset + bufferView.byteOffset], count * sizeof(XMFLOAT4X4));
			}
		}

		// process nodes and create hierarchy
		Entity rootEntity = ECS::CreateEntity();
		TransformComponent& rootTransform = newScene.GetOrCreateTransformByEntity(rootEntity);

		tinygltf::Scene& gltfScene = gltfModel.scenes[std::max(0, gltfModel.defaultScene)];
		for (int node : gltfScene.nodes) {
			LoadNode(node, rootEntity, loaderInfo);
		}

		// mapping bone to armature
		int armatureIndex = 0;
		for (auto gltfSkin : gltfModel.skins)
		{
			ArmatureComponent& armature = *newScene.mArmatures.GetComponentByIndex(armatureIndex++);

			size_t boneCount = gltfSkin.joints.size();
			armature.mSkiningBones.resize(boneCount);

			for (size_t index = 0; index < boneCount; index++)
			{
				armature.mSkiningBones[index] = loaderInfo.nodeEntityMap[gltfSkin.joints[index]];
			}
		}

		// gltf默认使用RH，转到LH
		if (true)
		{
			XMFLOAT3 rootScale = rootTransform.GetScaleLocal();
			rootScale.z *= -1.0f;
			rootTransform.SetScaleLocal(rootScale);
		}

		// texture已经在material中引用，所以清除临时的引用数据
		loaderInfo.loadedTextures.clear();

		// update scene
		Scene::GetScene().Merge(loaderInfo.scene);
	}
}
}