#include "mesh.h"
#include "renderer\renderer.h"
#include "renderer\RHI\rhiFactory.h"

namespace Cjing3D
{
	MeshComponent::MeshComponent() :
		Component(ComponentType_Mesh)
	{
	}

	MeshComponent::~MeshComponent()
	{
		mVertexBufferPos.reset();
		mVertexBufferTex.reset();
		mIndexBuffer.reset();
	}

	void MeshComponent::SetupRenderData(GraphicsDevice& device)
	{
		mVertexBufferPos = std::make_unique<GPUBuffer>();
		mVertexBufferTex = std::make_unique<GPUBuffer>();
		mIndexBuffer = std::make_unique<GPUBuffer>();

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
			// 记录每个顶点所属于的subset的index
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
				F32x3 normal = mVertexNormals.size() > 0 ? mVertexNormals[i] : F32x3(0.0f, 0.0f, 0.0f);
				normal = F32x3Normalize(normal);
				U32 index = subsetIndices[i];

				auto& vertex = vertices[i];
				vertex.Setup(pos, normal, index);

				mMinPos = F32x3Min(mMinPos, pos);
				mMaxPos = F32x3Max(mMaxPos, pos);
			}

			mVertexBufferPos.reset(new GPUBuffer);
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

	void MeshComponent::Serialize(Archive& archive, U32 seed)
	{
		archive >> mVertexPositions;
		archive >> mVertexNormals;
		archive >> mVertexTexcoords;
		archive >> mIndices;

		U32 subsetSize = 0;
		archive >> subsetSize;
		if (subsetSize > 0)
		{
			mSubsets.resize(subsetSize);
			for (int i = 0; i < subsetSize; i++)
			{
				mSubsets[i].mMaterialID = SerializeEntity(archive, seed);

				archive >> mSubsets[i].mIndexCount;
				archive >> mSubsets[i].mIndexOffset;
			}
		}

		SystemContext& systemContext = SystemContext::GetSystemContext();
		Renderer& renderer = systemContext.GetSubSystem<Renderer>();
		SetupRenderData(renderer.GetDevice());
	}

	void MeshComponent::Unserialize(Archive& archive) const
	{
		archive << mVertexPositions;
		archive << mVertexNormals;
		archive << mVertexTexcoords;
		archive << mIndices;

		archive << mSubsets.size();
		for (const auto& subset : mSubsets)
		{
			archive << subset.mMaterialID;
			archive << subset.mIndexCount;
			archive << subset.mIndexOffset;
		}
	}

	void MeshComponent::VertexPosNormalSubset::Setup(F32x3 pos, F32x3 normal, U32 subsetIndex)
	{
		mPos = pos;
		Setup(normal, subsetIndex);
	}

	void MeshComponent::VertexPosNormalSubset::Setup(F32x3 normal, U32 subsetIndex)
	{
		// normal 必须 normalize
		// 一个32位依次存储 subsetindex, normal_z, normal_y, normal_x
		Debug::CheckAssertion(subsetIndex < 256, "Invalid subset index.");

		mNormalSubsetIndex = 0;
		mNormalSubsetIndex |= (U32)((normal[0] * 0.5f + 0.5f) * 255.0f) << 0;
		mNormalSubsetIndex |= (U32)((normal[1] * 0.5f + 0.5f) * 255.0f) << 8;
		mNormalSubsetIndex |= (U32)((normal[2] * 0.5f + 0.5f) * 255.0f) << 16;
		mNormalSubsetIndex |= ((subsetIndex && 0x000000FF) << 24);
	}
}