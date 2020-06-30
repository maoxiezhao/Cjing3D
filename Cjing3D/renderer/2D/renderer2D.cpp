#include "renderer2D.h"
#include "renderer\renderer.h"
#include "renderer\RHI\rhiFactory.h"
#include "renderer\preset\renderPreset.h"
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

		std::vector<Sprite*> mRenderRequestSprites;

		std::vector<TextDrawable*> mPersistentTexts;
		std::vector<TextDrawable*> mRenderRequestTexts;

		std::vector<RenderLayer2D> mRenderLayers{ 1 };
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
			RenderPreset& renderPreset = Renderer::GetRenderPreset();
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
			desc.mBlendState = renderPreset.GetBlendState(BlendStateID_Transpranent);
			desc.mDepthStencilState = renderPreset.GetDepthStencilState(DepthStencilStateID_DepthNone);
			desc.mRasterizerState = renderPreset.GetRasterizerState(RasterizerStateID_Image);

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

				// ��ǰbatchInstance��Ȼ��array���
				mRenderBatchInstances.back()->mInstanceCount++;
				currentInstanceCount++;
			}

			if (mRenderBatchInstances.size() > 0)
			{
				graphicsDevice.BindSamplerState(SHADERSTAGES_PS, *Renderer::GetRenderPreset().GetSamplerState(SamplerStateID_LinearClampGreater), SAMPLER_LINEAR_CLAMP_SLOT);
			}

			for (auto& bathInstance : mRenderBatchInstances)
			{
				// TODO: sprite����֧���Զ���customShader
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
					bathInstance->mDataOffset	// ��Ϊ���е�batch����һ��buffer�������ṩoffset
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

		void RenderSprites()
		{
			LinearAllocator& frameAllocator = Renderer::GetFrameAllocator(Renderer::FrameAllocatorType_Render);
			auto flushQueue = [&]()->void
			{
				//mSpriteRenderBatchQueue.Sort(SpriteRenderBatchQueue::BackToFront);
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

		///////////////////////////////////////////////////////////////////////////////////
	}

	void Initialize()
	{
		Logger::Info("Renderer2D Initialized");

		Font::Initialize();

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

		mSpriteRenderBatchQueue.Clear();
		mPersistentTexts.clear();
		mRenderRequestTexts.clear();
		mRenderLayers.clear();

		mSpriteBuffer.Clear();
		mVertexBuffer.Clear();
		mIndexBuffer.Clear();
		mSpriteBatchPSO.Clear();

		Font::Uninitialize();
	}

	void Render2D()
	{
		for (auto& layer : mRenderLayers)
		{
			if (layer.mItems.empty()) {
				continue;
			}

			for (auto& item : layer.mItems)
			{
				if (item.mSprite != nullptr && item.mSprite->IsVisible()) {
					mRenderRequestSprites.push_back(item.mSprite);
				}

				if (item.mText != nullptr && item.mText->IsVisible()) 
				{
					if (!mRenderRequestSprites.empty()) {
						RenderSprites();
					}

					item.mText->Draw();
				}

				if (item.mType == RenderItem2D::RenderItemType_Scissor)
				{
					if (!mRenderRequestSprites.empty()) {
						RenderSprites();
					}

					Renderer::GetDevice().BindScissorRects(1, &item.mScissorRect);
				}
			}

			if (!mRenderRequestSprites.empty()) {
				RenderSprites();
			}
		}

		// clear nullptr and temp items
		CleanLayers();
	}

	void AddSprite(Sprite* sprite, bool isPersistent, const StringID& layerName)
	{
		auto it = std::find_if(mRenderLayers.begin(), mRenderLayers.end(), [layerName](RenderLayer2D& layer) {
			return layer.mLayerName == layerName;
			});
		if (it == mRenderLayers.end()) {
			return;
		}

		auto& item = (*it).mItems.emplace_back();
		item.mIsPersistent = isPersistent;
		item.mSprite = sprite;
		item.mType = RenderItem2D::RenderItemType_Sprite;
	}

	void RemoveSprite(Sprite* sprite)
	{
		if (sprite == nullptr) {
			return;
		}

		for (auto& layer : mRenderLayers)
		{
			for (auto& item : layer.mItems)
			{
				if (item.mSprite == sprite) {
					item.mSprite = nullptr;
				}
			}
		}
	}

	void RemoveSprite(Sprite* sprite, const StringID& layerName)
	{
		auto it = std::find_if(mRenderLayers.begin(), mRenderLayers.end(), [layerName](RenderLayer2D& layer) {
			return layer.mLayerName == layerName;
			});
		if (it == mRenderLayers.end()) {
			return;
		}

		for (auto& item : (*it).mItems)
		{
			if (item.mSprite == sprite) {
				item.mSprite = nullptr;
			}
		}
	}

	void RenderTextDrawables()
	{
		// �Ƚ�presistent texts��ӵ�renderRequestTexts��
		for (TextDrawable* text : mPersistentTexts)
		{
			if (text != nullptr) {
				mRenderRequestTexts.push_back(text);
			}
		}

		// render text
		for (TextDrawable* text : mRenderRequestTexts)
		{
			if (text != nullptr && text->IsVisible()) {
				text->Draw();
			}
		}
		mRenderRequestTexts.clear();
	}

	void AddTextDrawable(TextDrawable* text, bool isPersistent, const StringID& layerName)
	{
		auto it = std::find_if(mRenderLayers.begin(), mRenderLayers.end(), [layerName](RenderLayer2D& layer) {
			return layer.mLayerName == layerName;
			});
		if (it == mRenderLayers.end()) {
			return;
		}

		auto& item = (*it).mItems.emplace_back();
		item.mIsPersistent = isPersistent;
		item.mText = text;
		item.mType = RenderItem2D::RenderItemType_Font;
	}

	void RemoveTextDrawable(TextDrawable* text)
	{
		if (text == nullptr) {
			return;
		}

		for (auto& layer : mRenderLayers)
		{
			for (auto& item : layer.mItems)
			{
				if (item.mText == text) {
					item.mText = nullptr;
				}
			}
		}
	}

	void RemoveTextDrawable(TextDrawable* text, const StringID& layerName)
	{
		auto it = std::find_if(mRenderLayers.begin(), mRenderLayers.end(), [layerName](RenderLayer2D& layer) {
			return layer.mLayerName == layerName;
			});
		if (it == mRenderLayers.end()) {
			return;
		}

		for (auto& item : (*it).mItems)
		{
			if (item.mText == text) {
				item.mText = nullptr;
			}
		}
	}

	void AddScissorRect(const RectInt& rect, const StringID& layerName)
	{
		auto it = std::find_if(mRenderLayers.begin(), mRenderLayers.end(), [layerName](RenderLayer2D& layer) {
			return layer.mLayerName == layerName;
			});
		if (it == mRenderLayers.end()) {
			return;
		}

		auto& item = (*it).mItems.emplace_back();
		item.mIsPersistent = false;
		item.mScissorRect = rect;
		item.mType = RenderItem2D::RenderItemType_Scissor;
	}

	void AddLayer(const StringID& name)
	{
		auto it = std::find_if(mRenderLayers.begin(), mRenderLayers.end(), [name](RenderLayer2D& layer) {
			return layer.mLayerName == name;
			});
		if (it != mRenderLayers.end()) {
			return;
		}

		RenderLayer2D& layer = mRenderLayers.emplace_back();
		layer.mLayerName = name;
		layer.mSort = (U32)mRenderLayers.size() - 1;
	}

	void RemoveLayer(const StringID& name)
	{
		auto it = std::find_if(mRenderLayers.begin(), mRenderLayers.end(), [name](RenderLayer2D& layer) {
			return layer.mLayerName == name;
			});
		if (it != mRenderLayers.end()) {
			mRenderLayers.erase(it);
		}
	}

	void SortLayers()
	{
		if (mRenderLayers.empty()) {
			return;
		}

		std::sort(mRenderLayers.begin(), mRenderLayers.end(), [](RenderLayer2D& l1, RenderLayer2D& l2) {
			return l1.mSort < l2.mSort;
		});
	}

	void CleanLayers()
	{
		for (auto& layer : mRenderLayers)
		{
			if (layer.mItems.empty()) {
				continue;
			}

			std::vector<RenderItem2D> itemsToRetain(0);
			for (auto& item : layer.mItems)
			{
				if (item.mIsPersistent && (item.mSprite != nullptr || item.mText != nullptr)) {
					itemsToRetain.push_back(item);
				}
			}
			layer.mItems.clear();
			layer.mItems = itemsToRetain;
		}
	}
}
}