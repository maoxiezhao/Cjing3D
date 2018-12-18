#pragma once

#include "renderer\RHI\rhiPipeline.h"

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
		U32 mMeshGUID;
		U32 mDataOffset;
	};

	class RenderBatch
	{
	public:
		RenderBatch();

		void Init(Renderable& renderable, Actor* actor);

		U32 GetHash()const { return mHash; }
		U32 GetMeshGUID()const { return mHash; }

		// TODO
		Actor* GetActor() { return mActor; }
	private:
		// 当前仅以MeshGUID作为Hash
		U32 mHash;
		Actor* mActor;
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

	private:
		std::vector<RenderBatch*> mRenderBatch;
	};
}