#pragma once

#include "system\ecsSystem.h"
#include "renderer\RHI\rhiResource.h"
#include "renderer\RHI\device.h"

#include <functional>

namespace Cjing3D
{
	class Scene;
	class GraphicsDevice;
	

	// 延迟的Mip生成器
	struct MipGenerateOption
	{
		int arrayIndex = -1;
	};

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

		DeferredMIPGenerator();

		void AddTexture(Texture2D& texture, MipGenerateOption option = MipGenerateOption());
		void UpdateMipGenerating(CommandList cmd);
		void GenerateMipChain(CommandList cmd, Texture2D& texture, MIPGENFILTER filter, MipGenerateOption option = MipGenerateOption());

	private:
		// TODO; 可能存在某些纹理提前释放问题
		std::vector<std::pair<Texture2D*, MipGenerateOption>> mMipGenDeferredArray;
		const U32 maxGenerateCountPerFrame = 16;	// TODO
	};
	struct alignas(16) RenderInstance 
	{
		XMFLOAT4A mat0;
		XMFLOAT4A mat1;
		XMFLOAT4A mat2;
		XMUINT4   userdata;

		RenderInstance(const XMFLOAT4X4& mat, const XMFLOAT4 color = XMFLOAT4(1, 1, 1, 1))
		{
			Setup(mat, color);
		}

		inline void Setup(const XMFLOAT4X4& mat, const XMFLOAT4 color = XMFLOAT4(1, 1, 1, 1))
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

			userdata.x = Color4::Convert(color).GetRGBA();
			userdata.y = 0;
			userdata.z = 0;
			userdata.w = 0;
		}

		inline void SetUserdata(U32 userdataX, U32 userdataY, U32 userdataZ, U32 userdataW)
		{
			userdata.x = userdataX;
			userdata.y = userdataY;
			userdata.z = userdataZ;
			userdata.w = userdataW;
		}
	};

	struct RenderBatchInstance
	{
		ECS::Entity mMeshEntity = ECS::INVALID_ENTITY;
		U32 mDataOffset = 0;
		U32 mInstanceCount = 0;
	};

	class RenderBatch
	{
	public:
		RenderBatch();
		~RenderBatch();

		void Init(ECS::Entity objectEntity, ECS::Entity meshEntity, F32 distance);

		U32 GetHash()const { return mHash; }
		U32 GetMeshGUID()const { return mHash; }
		ECS::Entity GetMeshEntity()const { return mMeshEntity;}
		ECS::Entity GetObjectEntity()const { return mObjectEntity; }

	private:
		U32 mHash;
		ECS::Entity mMeshEntity;
		ECS::Entity mObjectEntity;
	};

	class RenderQueue
	{
	public:
		enum SortMethod
		{
			FrontToBack,
			BackToFront,
		};

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
		
		void Sort(SortMethod sortMethod)
		{
			if (mRenderBatch.empty() == false)
			{
				if (sortMethod == FrontToBack)
				{
					std::sort(mRenderBatch.begin(), mRenderBatch.end(), [](const RenderBatch* a, const RenderBatch* b) {
						return a->GetHash() < b->GetHash();
					});
				}
				else
				{
					std::sort(mRenderBatch.begin(), mRenderBatch.end(), [](const RenderBatch* a, const RenderBatch* b) {
						return a->GetHash() > b->GetHash();
					});
				}
			}
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

	struct InstanceHandler
	{
	public:
		using CheckConditionFunc = std::function<bool(U32, ECS::Entity, Scene&)>;
		using ProcessInstanceFunc = std::function<void(U32, RenderInstance&)>;

		CheckConditionFunc checkCondition_ = nullptr;
		ProcessInstanceFunc processInstance_ = nullptr;
	};
}