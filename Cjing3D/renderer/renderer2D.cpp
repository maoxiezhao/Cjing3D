#include "renderer2D.h"
#include "renderer\renderer.h"
#include "renderer\RHI\rhiFactory.h"
#include "renderer\rhiResourceManager.h"
#include "renderer\pipelineStates\pipelineStateManager.h"

namespace Cjing3D
{
namespace Renderer2D {
	// sprite batch rendering v0.1
	// TODO: 
	// 1. support layer
	// 2. refactor code structure
	namespace {
		GPUBuffer mSpriteBuffer;
		GPUBuffer mVertexBuffer;
		GPUBuffer mIndexBuffer;
		PipelineState mSpriteBatchPSO;

		size_t MaxBatchSize = 2048;

		std::vector<Sprite*> mPersistentSprites;
		std::vector<Sprite*> mRenderRequestSprites;
	}

	namespace {
		struct SpriteBatchInstance
		{
			Texture2D* mTexture = nullptr;
			U32 mDataOffset = 0;
			U32 mInstanceCount = 0;
		};

		struct SpriteRenderInstanceData
		{
			XMFLOAT4 translation;
			XMFLOAT4 sourceRect;
			XMFLOAT4 originRotationDepth;
			XMFLOAT4 color;
			XMFLOAT4 inverseTextureSize;

			void Setup(Sprite* sprite)
			{
				RenderImage::ImageParams& params = sprite->GetImageParams();
				translation = XMFLOAT4(
					params.mPos[0],
					params.mPos[1],
					params.mScale[0] * params.mSize[0],
					params.mScale[1] * params.mSize[1]
				);

				sourceRect = XMConvert(params.mTexSource);

				originRotationDepth = XMFLOAT4(
					params.mPivot[0],
					params.mPivot[1],
					params.mRotation,
					params.mLayerDepth
				);

				color = XMConvert(params.mColor);

				F32x2 invSize = sprite->GetInverseTextureSize();
				inverseTextureSize = XMFLOAT4(
					invSize[0],
					invSize[1],
					0.0f, 0.0f
				);
			}
		};

		struct VertexPosTex
		{
			F32x2 mPos = F32x2(0.0f, 0.0f);
			F32x2 mTex = F32x2(0.0f, 0.0f);

			void Setup(const F32x2& pos, const F32x2& tex)
			{
				mPos = pos;
				mTex = tex;
			}
			static const FORMAT format = FORMAT::FORMAT_R32G32B32A32_FLOAT;
		};

		struct SpriteRenderBatch
		{
			U32 mHash = 0;
			Sprite* mSprite = nullptr;

			void Init(Sprite* sprite)
			{
				if (sprite != nullptr && sprite->GetTextureResource() != nullptr) {
					mHash = sprite->GetTextureResource()->GetGUID();	// TODO
				}
				mSprite = sprite;
			}
			U32 GetHash() const
			{
				return mHash;
			}
		};

		class SpriteRenderBatchQueue
		{
		public:
			enum SortMethod
			{
				FrontToBack,
				BackToFront,
			};

			void AddBatch(SpriteRenderBatch* batch)
			{
				mRenderBatchQueue.push_back(batch);
			}

			void Clear()
			{
				mRenderBatchQueue.clear();
			}

			void Sort(SortMethod sortMethod)
			{
				if (mRenderBatchQueue.empty() == false)
				{
					if (sortMethod == FrontToBack)
					{
						std::sort(mRenderBatchQueue.begin(), mRenderBatchQueue.end(),
							[](const SpriteRenderBatch* a, const SpriteRenderBatch* b) {
								return a->GetHash() < b->GetHash();
							});
					}
					else
					{
						std::sort(mRenderBatchQueue.begin(), mRenderBatchQueue.end(),
							[](const SpriteRenderBatch* a, const SpriteRenderBatch* b) {
								return a->GetHash() > b->GetHash();
							});
					}
				}
			}

			bool IsEmpty()const
			{
				return mRenderBatchQueue.empty();
			}

			size_t GetBatchCount()const
			{
				return mRenderBatchQueue.size();
			}

			std::vector<SpriteRenderBatch*> mRenderBatchQueue;
		};

		void InitializeFrame2DBuffer()
		{
			GPUBufferDesc desc = {};
			desc.mUsage = USAGE_DYNAMIC;
			desc.mCPUAccessFlags = CPU_ACCESS_WRITE;
			desc.mBindFlags = BIND_CONSTANT_BUFFER;
			desc.mByteWidth = sizeof(SpriteCB);

			const HRESULT result = Renderer::GetDevice().CreateBuffer(&desc, mSpriteBuffer, nullptr);
			Debug::ThrowIfFailed(result, "failed to create sprite constant buffer:%08x", result);
			Renderer::GetDevice().SetResourceName(mSpriteBuffer, "SpriteCB");
		}

		void InitializePipelineState()
		{
			RhiResourceManager& rhiResourceManager = Renderer::GetStateManager();
			ResourceManager& resourceManager = GlobalGetSubSystem<ResourceManager>();

			const std::string shaderPath = resourceManager.GetStandardResourceDirectory(Resource_Shader);
			VertexLayoutDesc posLayout[] =
			{
				{ "POSITION_TEXCOORD",     0u, VertexPosTex::format, 0u, APPEND_ALIGNED_ELEMENT,  INPUT_PER_VERTEX_DATA , 0u },

				// instance
				{ "TRANSLATION",           0u, FORMAT_R32G32B32A32_FLOAT, 1u, APPEND_ALIGNED_ELEMENT,  INPUT_PER_INSTANCE_DATA , 1u },
				{ "SOURCERECT",            0u, FORMAT_R32G32B32A32_FLOAT, 1u, APPEND_ALIGNED_ELEMENT,  INPUT_PER_INSTANCE_DATA , 1u },
				{ "ORIGIN_ROTATION_DEPTH", 0u, FORMAT_R32G32B32A32_FLOAT, 1u, APPEND_ALIGNED_ELEMENT,  INPUT_PER_INSTANCE_DATA , 1u },
				{ "COLOR",			       0u, FORMAT_R32G32B32A32_FLOAT, 1u, APPEND_ALIGNED_ELEMENT,  INPUT_PER_INSTANCE_DATA , 1u },
				{ "INVERSE_SIZE",		   0u, FORMAT_R32G32B32A32_FLOAT, 1u, APPEND_ALIGNED_ELEMENT,  INPUT_PER_INSTANCE_DATA , 1u }
			};
			auto vsinfo = Renderer::LoadVertexShaderInfo(shaderPath + "spriteVS.cso", posLayout, ARRAYSIZE(posLayout));
			PipelineStateDesc desc = {};
			desc.mInputLayout = vsinfo.mInputLayout;
			desc.mVertexShader = vsinfo.mVertexShader;
			desc.mPixelShader = Renderer::LoadShader(SHADERSTAGES_PS, shaderPath + "spritePS.cso");
			desc.mPrimitiveTopology = TRIANGLELIST;
			desc.mBlendState = rhiResourceManager.GetBlendState(BlendStateID_Transpranent);
			desc.mDepthStencilState = rhiResourceManager.GetDepthStencilState(DepthStencilStateID_DepthNone);
			desc.mRasterizerState = rhiResourceManager.GetRasterizerState(RasterizerStateID_Image);

			Renderer::GetDevice().CreatePipelineState(desc, mSpriteBatchPSO);
		}

		void ProcessRenderQueue(SpriteRenderBatchQueue& queue)
		{
			if (queue.IsEmpty() == true) {
				return;
			}

			LinearAllocator& frameAllocator = Renderer::GetFrameAllocator(Renderer::FrameAllocatorType_Render);
			GraphicsDevice& graphicsDevice = Renderer::GetDevice();

			size_t instanceSize = sizeof(SpriteRenderInstanceData);
			size_t allocSize = queue.mRenderBatchQueue.size() * instanceSize;
			GraphicsDevice::GPUAllocation instances = graphicsDevice.AllocateGPU(allocSize);

			U32 instancedBatchCount = 0;
			U32 prevBatchHash = -1;
			std::vector<SpriteBatchInstance*> mRenderBatchInstances;

			// visist render batch queue
			size_t currentInstanceCount = 0;
			for (size_t index = 0; index < queue.mRenderBatchQueue.size(); index++)
			{
				auto& batch = queue.mRenderBatchQueue[index];
				Sprite* sprite = batch->mSprite;
				if (sprite == nullptr) {
					continue;
				}

				U32 currentBatchHash = batch->GetHash();
				if (currentBatchHash != prevBatchHash)
				{
					prevBatchHash = currentBatchHash;
					instancedBatchCount++;

					SpriteBatchInstance* batchInstance = (SpriteBatchInstance*)frameAllocator.Allocate(sizeof(SpriteBatchInstance));
					batchInstance->mTexture = sprite->GetTexture();
					batchInstance->mDataOffset = instances.offset + index * instanceSize;
					batchInstance->mInstanceCount = 0;

					mRenderBatchInstances.push_back(batchInstance);
				}

				// setup instance data
				((SpriteRenderInstanceData*)instances.data)[currentInstanceCount].Setup(sprite);

				// 当前batchInstance必然在array最后
				mRenderBatchInstances.back()->mInstanceCount++;
				currentInstanceCount++;
			}

			if (mRenderBatchInstances.size() > 0)
			{
				graphicsDevice.BindSamplerState(SHADERSTAGES_PS, *Renderer::GetStateManager().GetSamplerState(SamplerStateID_LinearClampGreater), SAMPLER_LINEAR_CLAMP_SLOT);
			}

			for (auto& bathInstance : mRenderBatchInstances)
			{
				// TODO: sprite可以支持自定义customShader
				PipelineState& state = mSpriteBatchPSO;
				if (!state.IsValid()) {
					continue;
				}

				// bind vertex buffer
				GPUBuffer* vbs[] = {
					&mVertexBuffer,
					instances.buffer
				};
				U32 strides[] = {
					sizeof(VertexPosTex),
					(U32)instanceSize
				};
				U32 offsets[] = {
					0,
					bathInstance->mDataOffset	// 因为所有的batch公用一个buffer，所以提供offset
				};
				graphicsDevice.BindVertexBuffer(vbs, 0, ARRAYSIZE(vbs), strides, offsets);
				graphicsDevice.BindIndexBuffer(mIndexBuffer, IndexFormat::INDEX_FORMAT_32BIT, 0);
				graphicsDevice.BindPipelineState(state);
				graphicsDevice.BindGPUResource(SHADERSTAGES_PS, *bathInstance->mTexture, TEXTURE_SLOT_0);
				graphicsDevice.DrawIndexedInstances(6, bathInstance->mInstanceCount, 0, 0, 0);
			}

			graphicsDevice.UnAllocateGPU();
			frameAllocator.Free(instancedBatchCount * sizeof(SpriteBatchInstance));
		}

		///////////////////////////////////////////////////////////////////////////////////


		SpriteRenderBatchQueue mSpriteRenderBatchQueue;

		///////////////////////////////////////////////////////////////////////////////////
	}

	void Initialize()
	{
		Logger::Info("Renderer2D Initialized");

		GraphicsDevice& device = Renderer::GetDevice();
		{
			std::vector<U32> indices = {
				0, 1, 2,
				1, 3, 2
			};
			const auto result = CreateStaticIndexBuffer(device, mIndexBuffer, indices);
			Debug::ThrowIfFailed(result, "Failed to create index buffer:%08x", result);
		}
		{
			std::vector<VertexPosTex> vertices(4);
			vertices[0].Setup({ 0.0f, 0.0f }, { 0.0f, 0.0f });
			vertices[1].Setup({ 1.0f, 0.0f }, { 1.0f, 0.0f });
			vertices[2].Setup({ 0.0f, 1.0f }, { 0.0f, 1.0f });
			vertices[3].Setup({ 1.0f, 1.0f }, { 1.0f, 1.0f });

			const auto result = CreateBABVertexBuffer(device, mVertexBuffer, vertices);
			Debug::ThrowIfFailed(result, "Failed to create vertex buffer:%08x", result);
		}

		InitializeFrame2DBuffer();
		InitializePipelineState();
	}

	void Uninitialize()
	{
		Logger::Info("Renderer2D Uninitialized");

		mPersistentSprites.clear();
		mSpriteRenderBatchQueue.Clear();

		mSpriteBuffer.Clear();
		mVertexBuffer.Clear();
		mIndexBuffer.Clear();
		mSpriteBatchPSO.Clear();
	}

	void RenderSprites()
	{
		// 先将presistent sprite添加到renderRequestSpriteS中
		for (Sprite* sprite : mPersistentSprites)
		{
			if (sprite != nullptr) {
				mRenderRequestSprites.push_back(sprite);
			}
		}

		LinearAllocator& frameAllocator = Renderer::GetFrameAllocator(Renderer::FrameAllocatorType_Render);
		auto flushQueue = [&]()->void
		{
			mSpriteRenderBatchQueue.Sort(SpriteRenderBatchQueue::BackToFront);
			ProcessRenderQueue(mSpriteRenderBatchQueue);
			mSpriteRenderBatchQueue.Clear();

			frameAllocator.Free(mSpriteRenderBatchQueue.mRenderBatchQueue.size() * sizeof(RenderBatch));
		};

		// update sprite buffer
		SpriteCB cb = {};
		DirectX::XMStoreFloat4x4(&cb.gSpriteCameraVP, Renderer::GetScreenProjection());
		Renderer::GetDevice().UpdateBuffer(mSpriteBuffer, &cb, sizeof(SpriteCB));
		Renderer::GetDevice().BindConstantBuffer(SHADERSTAGES_VS, mSpriteBuffer, CB_GETSLOT_NAME(SpriteCB));

		// render sprites
		for (Sprite* sprite : mRenderRequestSprites)
		{
			if (sprite != nullptr && sprite->IsVisible())
			{
				SpriteRenderBatch* renderBatch = (SpriteRenderBatch*)frameAllocator.Allocate(sizeof(SpriteRenderBatch));
				renderBatch->Init(sprite);
				mSpriteRenderBatchQueue.AddBatch(renderBatch);

				if (mSpriteRenderBatchQueue.GetBatchCount() >= MaxBatchSize) {
					flushQueue();
				}
			}
		}

		if (!mSpriteRenderBatchQueue.IsEmpty()) {
			flushQueue();
		}

		mRenderRequestSprites.clear();
	}

	void AddSprite(Sprite* sprite)
	{
		mPersistentSprites.push_back(sprite);
	}

	void RemoveSprite(Sprite* sprite)
	{
		auto it = std::find(mPersistentSprites.begin(), mPersistentSprites.end(), sprite);
		if (it != mPersistentSprites.end()) {
			mPersistentSprites.erase(it);
		}
	}

	void RequestRenderSprite(Sprite* sprite)
	{
		mRenderRequestSprites.push_back(sprite);
	}
}
}