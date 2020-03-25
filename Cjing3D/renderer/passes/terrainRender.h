#pragma once

#include "renderer\RHI\rhiResource.h"
#include "utils\math.h"

namespace Cjing3D
{
	// 地形渲染批次
	struct TerrainRenderBatchInstance
	{
		GPUBuffer* mVertexBuffer = nullptr;
		GPUBuffer* mIndexBuffer = nullptr;

		U32 mIndiceCount = 0;
		U32 mDataOffset = 0;
		U32 mInstanceCount = 0;
	};

	struct alignas(16) RenderTerrainInstance
	{
		XMFLOAT4A mat0;
		XMFLOAT4A mat1;
		XMFLOAT4A mat2;
		XMFLOAT4A mat3;
		XMUINT4 terrain;

		RenderTerrainInstance(const XMFLOAT4X4& mat, const XMFLOAT4 color, U32 currentLodLevel, U32 edgeLodLevel, U32 vertexCount)
		{
			Setup(mat, color, currentLodLevel, edgeLodLevel, vertexCount);
		}

		inline void Setup(const XMFLOAT4X4 & mat, const XMFLOAT4 color, U32 currentLodLevel, U32 edgeLodLevel, U32 vertexCount)
		{
			// 在C++端矩阵存储方式为行为主，而在HLSL端则以列为主，且右乘矩阵
			// 所以在传递给HLSL端之前，这里直接将存储方式转换
			mat0.x = mat._11;
			mat0.y = mat._21;
			mat0.z = mat._31;
			mat0.w = mat._41;

			mat1.x = mat._12;
			mat1.y = mat._22;
			mat1.z = mat._32;
			mat1.w = mat._42;

			mat2.x = mat._13;
			mat2.y = mat._23;
			mat2.z = mat._33;
			mat2.w = mat._43;

			mat3.x = color.x;
			mat3.y = color.y;
			mat3.z = color.z;
			mat3.w = color.w;

			terrain.x = currentLodLevel;
			terrain.y = edgeLodLevel;
			terrain.z = vertexCount;
			terrain.w = 0;
		}
	};

	// 地形材质结构
	struct TerrainMaterial
	{
		Texture2DPtr weightTexture;
		Texture2DPtr detailTexture1;
		Texture2DPtr detailTexture2;
		Texture2DPtr detailTexture3;

		void Clear();
	};
}