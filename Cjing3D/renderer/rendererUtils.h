#pragma once

#include "renderer\RHI\rhiPipeline.h"
#include "renderer\components\mesh.h"

namespace Cjing3D
{
	class GraphicsDevice;
	class Renderable;
	class Actor;

	// 延迟的Mip生成器
	class DeferredMIPGenerator
	{
	public:
		enum MIPGENFILTER
		{
			MIPGENFILTER_POINT,
			MIPGENFILTER_LINEAR,
			MIPGENFILTER_LINEAR_MAXIMUM,
			MIPGENFILTER_GAUSSIAN,
		};

		DeferredMIPGenerator(GraphicsDevice& device);

		void AddTexture(RhiTexture2DPtr texture);
		void UpdateMipGenerating();

		void GenerateMaipChain(RhiTexture2D& texture, MIPGENFILTER filter);

	private:
		GraphicsDevice& mDevice;
		std::vector<RhiTexture2DPtr> mMipGenDeferredArray;
	};

	struct RenderBatchInstance
	{
		MeshPtr mMesh = nullptr;
		U32 mDataOffset = 0;
		U32 mInstanceCount = 0;
	};

	class RenderBatch
	{
	public:
		RenderBatch();
		~RenderBatch();

		void Init(Renderable& renderable);

		U32 GetHash()const { return mHash; }
		U32 GetMeshGUID()const { return mHash; }
		Renderable* GetRenderable() { return mRenderable; }
		MeshPtr GetMesh() { return mMesh; }

	private:
		// 当前仅以MeshGUID作为Hash
		U32 mHash;
		Renderable* mRenderable;
		MeshPtr mMesh;
	};

	class RenderQueue
	{
	public:
		void AddBatch(RenderBatch* batch)
		{
			mRenderBatch.push_back(batch);
		}

		void Clear()
		{
			for (auto& batch : mRenderBatch) {
				SAFE_DELETE(batch);
			}
			mRenderBatch.clear();
		}
		
		void Sort()
		{
		}
	
		bool IsEmpty()const 
		{
			return mRenderBatch.empty();
		}

		std::vector<RenderBatch*>& GetRenderBatches() 
		{
			return mRenderBatch;
		}

		size_t GetBatchCount()const 
		{
			return mRenderBatch.size();
		}

	private:
		std::vector<RenderBatch*> mRenderBatch;
	};
}