#pragma once

#include "world\component\componentInclude.h"

namespace Cjing3D {

	class MeshComponent : public Component
	{
	public:
		MeshComponent();

		std::vector<F32x3> mVertexPositions;
		std::vector<F32x3> mVertexNormals;
		std::vector<F32x2> mVertexTexcoords;
		std::vector<U32> mIndices;

		struct MeshSubset
		{
			ECS::Entity mMaterialID = ECS::INVALID_ENTITY;
			U32 mIndexOffset = 0;
			U32 mIndexCount = 0;
		};
		std::vector<MeshSubset> mSubsets;

		std::unique_ptr<GPUBuffer> mVertexBufferPos;
		std::unique_ptr<GPUBuffer> mVertexBufferTex;
		std::unique_ptr<GPUBuffer> mIndexBuffer;

		AABB mAABB;

	public:
		void SetupRenderData(GraphicsDevice& device);

		GPUBuffer& GetIndexBuffer() { return *mIndexBuffer; }
		GPUBuffer& GetVertexBufferPos() { return *mVertexBufferPos; }
		GPUBuffer& GetVertexBufferTex() { return *mIndexBuffer; }


		IndexFormat GetIndexFormat()const { return IndexFormat::INDEX_FORMAT_32BIT; }
		std::vector<MeshSubset>& GetSubsets() { return mSubsets; }

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
	};
}





