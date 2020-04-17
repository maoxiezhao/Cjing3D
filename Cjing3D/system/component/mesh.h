#pragma once

#include "system\component\componentInclude.h"

namespace Cjing3D {

	class MeshComponent : public Component
	{
	public:
		MeshComponent();
		virtual ~MeshComponent();

		std::vector<F32x3> mVertexPositions;
		std::vector<F32x3> mVertexNormals;
		std::vector<F32x2> mVertexTexcoords;
		std::vector<U32> mVertexColors;
		std::vector<U32> mIndices;
		std::vector<U32x4> mVertexBoneIndices;
		std::vector<F32x4> mVertexBoneWeights;

		ECS::Entity mArmature = ECS::INVALID_ENTITY;

		AABB mAABB;

		struct MeshSubset
		{
			ECS::Entity mMaterialID = ECS::INVALID_ENTITY;
			U32 mIndexOffset = 0;
			U32 mIndexCount = 0;
		};
		std::vector<MeshSubset> mSubsets;

		std::unique_ptr<GPUBuffer> mVertexBufferPos;
		std::unique_ptr<GPUBuffer> mVertexBufferTex;
		std::unique_ptr<GPUBuffer> mVertexBufferColor;
		std::unique_ptr<GPUBuffer> mIndexBuffer;
		std::unique_ptr<GPUBuffer> mVertexBufferBoneIndexWeight;
		std::unique_ptr<GPUBuffer> mVertexBufferStreamoutPos;

	public:
		void SetupRenderData(GraphicsDevice& device);

		GPUBuffer* GetIndexBuffer() { return mIndexBuffer.get(); }
		GPUBuffer* GetVertexBufferPos() { return mVertexBufferPos.get(); }
		GPUBuffer* GetVertexBufferTex() { return mVertexBufferTex.get(); }
		GPUBuffer* GetVertexBufferColor() { return mVertexBufferColor.get(); }
		GPUBuffer* GetVertexBufferBoneIndexWeight() { return mVertexBufferBoneIndexWeight.get(); }
		GPUBuffer* GetVertexBufferStreamoutPos() { return mVertexBufferStreamoutPos.get(); }
		GPUBuffer* GetFinalVertexBufferPos();

		IndexFormat GetIndexFormat()const { return IndexFormat::INDEX_FORMAT_32BIT; }
		std::vector<MeshSubset>& GetSubsets() { return mSubsets; }

		bool IsSkinned()const { return mArmature != ECS::INVALID_ENTITY; }

		virtual void Serialize(Archive& archive, U32 seed = 0);
		virtual void Unserialize(Archive& archive)const;

	public:

		// 顶点结构体，因为法线值可以以（0-256）存储,故法线坐标可以用3个8位
		// 表示，同时加上subsetIndex存储在第四个8位中
		struct VertexPosNormalSubset
		{
			F32x3 mPos = F32x3(0.0f, 0.0f, 0.0f);
			//  subsetindex, normal_z, normal_y, normal_x
			U32 mNormalSubsetIndex = 0;

			void Setup(F32x3 pos, F32x3 normal, U32 subsetIndex);
			void Setup(F32x3 normal, U32 subsetIndex);

			static const FORMAT format = FORMAT::FORMAT_R32G32B32A32_FLOAT;
		};

		// TODO:可以用两个Half float 表示的纹理坐标
		struct VertexTex
		{
			F32x2 mTex = F32x2(0.0f, 0.0f);
			static const FORMAT format = FORMAT::FORMAT_R32G32_FLOAT;
		};

		struct VertexColor
		{
			U32 mColor = 0;
			static const FORMAT format = FORMAT::FORMAT_R8G8B8A8_UNORM;
		};

		struct VertexBoneIndexWeight
		{
			U64 mIndices = 0u;
			U64 mWeights = 0u;

			void Setup(U32x4 boneIndices, F32x4 boneWeights);
		};
	};
}





