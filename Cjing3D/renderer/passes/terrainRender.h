#pragma once

#include "renderer\RHI\rhiResource.h"
#include "utils\math.h"

namespace Cjing3D
{
	// ������Ⱦ����
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
		XMFLOAT4A lcoalTrans;
		XMUINT4 terrain;

		RenderTerrainInstance(const XMFLOAT4 currentLcoalTrans, U32 currentLodLevel, U32 edgeLodLevel, U32 vertexCount)
		{
			Setup(currentLcoalTrans, currentLodLevel, edgeLodLevel, vertexCount);
		}

		inline void Setup(const XMFLOAT4 currentLcoalTrans, U32 currentLodLevel, U32 edgeLodLevel, U32 vertexCount)
		{
			// ��C++�˾���洢��ʽΪ��Ϊ��������HLSL��������Ϊ�������ҳ˾���
			// �����ڴ��ݸ�HLSL��֮ǰ������ֱ�ӽ��洢��ʽת��
			lcoalTrans.x = currentLcoalTrans.x;
			lcoalTrans.y = currentLcoalTrans.y;
			lcoalTrans.z = currentLcoalTrans.z;
			lcoalTrans.w = currentLcoalTrans.w;

			terrain.x = currentLodLevel;
			terrain.y = edgeLodLevel;
			terrain.z = vertexCount;
			terrain.w = 0;
		}
	};

	// ���β��ʽṹ
	struct TerrainMaterial
	{
		Texture2DPtr weightTexture;
		Texture2DPtr detailTexture1;
		Texture2DPtr detailTexture2;
		Texture2DPtr detailTexture3;

		void Clear();
	};
}