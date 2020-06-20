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
		ClearRenderData();
	}

	void MeshComponent::SetupRenderData(GraphicsDevice& device)
	{
		ClearRenderData();

		// setup index buffer
		{
			const auto result = CreateStaticIndexBuffer(device, mIndexBuffer, mIndices);
			Debug::ThrowIfFailed(result, "Failed to create index buffer:%08x", result);
		}

		F32x3 mMinPos = { FLT_MAX, FLT_MAX, FLT_MAX };
		F32x3 mMaxPos = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

		// setup vertex pos buffer
		// format: pos + normal + subsetindex
		{
			// 创建VertexPosNormalSubset结构
			std::vector<VertexPosNormalSubset> vertices(mVertexPositions.size());
			for (int i = 0; i < mVertexPositions.size(); i++)
			{
				F32x3 pos = mVertexPositions[i];
				F32x3 normal = mVertexNormals.size() > 0 ? mVertexNormals[i] : F32x3(0.0f, 0.0f, 0.0f);
				normal = F32x3Normalize(normal);

				auto& vertex = vertices[i];
				vertex.Setup(pos, normal, 0);

				mMinPos = F32x3Min(mMinPos, pos);
				mMaxPos = F32x3Max(mMaxPos, pos);
			}

			const auto result = CreateBABVertexBuffer(device, mVertexBufferPos, vertices);
			Debug::ThrowIfFailed(result, "Failed to create vertex buffer:%08x", result);
		}

		mAABB = AABB(XMLoad(mMinPos), XMLoad(mMaxPos));

		// vertex texcoords
		if (mVertexTexcoords.empty() == false)
		{
			const auto result = CreateStaticVertexBuffer(device, mVertexBufferTex, mVertexTexcoords, VertexTex::format);
			Debug::ThrowIfFailed(result, "Failed to create vertex texcoords buffer:%08x", result);
		}

		// vertex colors
		if (mVertexColors.empty() == false)
		{
			const auto result = CreateStaticVertexBuffer(device, mVertexBufferColor, mVertexColors, VertexColor::format);
			Debug::ThrowIfFailed(result, "Failed to create vertex colors buffer:%08x", result);
		}

		// vertex bone indices and weights
		if (!mVertexBoneIndices.empty())
		{
			U32 boneCount = 0;
			std::vector<VertexBoneIndexWeight> bones(mVertexBoneIndices.size());
			for (int index = 0; index < mVertexBoneIndices.size(); index++)
			{
				U32x4 boneIndices = mVertexBoneIndices[index];
				F32x4 boneWeights = mVertexBoneWeights[index];

				float weightSum = boneWeights[0] + boneWeights[1] + boneWeights[2] + boneWeights[3];
				if (weightSum <= 0) {
					continue;
				}

				boneWeights /= (F32)weightSum;
				bones[boneCount++].Setup(boneIndices, boneWeights);
			}

			// USAGE_IMMUTABLE
			// GPU: read + write
			// CPU: no read + no write
			{
				const auto result = CreateBABVertexBuffer(device, mVertexBufferBoneIndexWeight, bones, BIND_SHADER_RESOURCE, USAGE_IMMUTABLE);
				Debug::ThrowIfFailed(result, "Failed to create bone indexWeight buffer:%08x", result);
			}
			// create streamout buffer
			{
				const auto result = CreateEmptyBABVertexBuffer<VertexPosNormalSubset>(device, mVertexBufferStreamoutPos, mVertexPositions.size(), BIND_VERTEX_BUFFER | BIND_SHADER_RESOURCE | BIND_UNORDERED_ACCESS);
				Debug::ThrowIfFailed(result, "Failed to create streamout vertex buffer:%08x", result);
			}
		}
	}

	void MeshComponent::ClearRenderData()
	{
		mVertexBufferPos.Clear();
		mVertexBufferTex.Clear();
		mVertexBufferColor.Clear();
		mIndexBuffer.Clear();
		mVertexBufferBoneIndexWeight.Clear();
		mVertexBufferStreamoutPos.Clear();
	}

	GPUBuffer* MeshComponent::GetFinalVertexBufferPos()
	{
		if (mVertexBufferStreamoutPos.IsValid()) 
		{
			return &mVertexBufferStreamoutPos;
		}
		else 
		{
			return &mVertexBufferPos;
		}
	}

	void MeshComponent::Serialize(Archive& archive, U32 seed)
	{
		archive >> mVertexPositions;
		archive >> mVertexNormals;
		archive >> mVertexTexcoords;
		archive >> mVertexColors;
		archive >> mIndices;
		archive >> mVertexBoneIndices;
		archive >> mVertexBoneWeights;

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

		SetupRenderData(Renderer::GetDevice());
	}

	void MeshComponent::Unserialize(Archive& archive) const
	{
		archive << mVertexPositions;
		archive << mVertexNormals;
		archive << mVertexTexcoords;
		archive << mVertexColors;
		archive << mIndices;
		archive << mVertexBoneIndices;
		archive << mVertexBoneWeights;

		archive << mSubsets.size();
		for (const auto& subset : mSubsets)
		{
			archive << subset.mMaterialID;
			archive << subset.mIndexCount;
			archive << subset.mIndexOffset;
		}
	}

	void MeshComponent::VertexPosNormalSubset::Setup(F32x3 pos, F32x3 normal, U32 extraData)
	{
		mPos = pos;
		Setup(normal, extraData);
	}

	void MeshComponent::VertexPosNormalSubset::Setup(F32x3 normal, U32 extraData)
	{
		// normal 必须 normalize
		// 一个32位依次存储 extraData, normal_z, normal_y, normal_x
		Debug::CheckAssertion(extraData < 256, "Invalid subset index.");

		mNormalSubsetIndex = 0;
		mNormalSubsetIndex |= (U32)((normal[0] * 0.5f + 0.5f) * 255.0f) << 0;
		mNormalSubsetIndex |= (U32)((normal[1] * 0.5f + 0.5f) * 255.0f) << 8;
		mNormalSubsetIndex |= (U32)((normal[2] * 0.5f + 0.5f) * 255.0f) << 16;
		mNormalSubsetIndex |= ((extraData && 0x000000FF) << 24);
	}

	void MeshComponent::VertexBoneIndexWeight::Setup(U32x4 boneIndices, F32x4 boneWeights)
	{
		mIndices = 0u;
		mIndices |= U64(boneIndices[0]) << 0;
		mIndices |= U64(boneIndices[1]) << 16;
		mIndices |= U64(boneIndices[2]) << 32;
		mIndices |= U64(boneIndices[3]) << 48;

		mWeights = 0u;
		mWeights |= U64(boneWeights[0] * 65535.0f) << 0;
		mWeights |= U64(boneWeights[1] * 65535.0f) << 16;
		mWeights |= U64(boneWeights[2] * 65535.0f) << 32;
		mWeights |= U64(boneWeights[3] * 65535.0f) << 48;
	}
}