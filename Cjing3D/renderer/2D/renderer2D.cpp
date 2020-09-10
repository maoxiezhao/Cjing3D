#include "renderer2D.h"
#include "renderer\renderer.h"
#include "renderer\RHI\rhiFactory.h"
#include "renderer\preset\renderPreset.h"
#include "renderer\pipelineStates\pipelineStateManager.h"

#include<numeric>

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

			void Setup(Sprite* sprite, const F32x2& pos, const F32x2& size, const F32x4& texRect)
			{
				RenderImage::ImageParams& params = sprite->GetImageParams();
				translation = XMFLOAT4(
					pos[0],
					pos[1],
					size[0],
					size[1]
				);

				sourceRect = XMConvert(texRect);

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
			Texture2D* GetTexture()
			{
				return mSprite != nullptr ? mSprite->GetTexture() : nullptr;
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
			auto vsinfo = Renderer::LoadVertexShaderInfo("spriteVS.cso", posLayout, ARRAYSIZE(posLayout));
			PipelineStateDesc desc = {};
			desc.mInputLayout = vsinfo.mInputLayout;
			desc.mVertexShader = vsinfo.mVertexShader;
			desc.mPixelShader = Renderer::LoadShader(SHADERSTAGES_PS, "spritePS.cso");
			desc.mPrimitiveTopology = TRIANGLELIST;
			desc.mBlendState = renderPreset.GetBlendState(BlendStateID_Transpranent);
			desc.mDepthStencilState = renderPreset.GetDepthStencilState(DepthStencilStateID_DepthNone);
			desc.mRasterizerState = renderPreset.GetRasterizerState(RasterizerStateID_Image);

			Renderer::GetDevice().CreatePipelineState(desc, mSpriteBatchPSO);
		}

		void ProcessRenderQueue(CommandList cmd, SpriteRenderBatchQueue& queue)
		{
			if (queue.IsEmpty()) {
				return;
			}

			LinearAllocator& renderAllocator = Renderer::GetRenderAllocator(cmd);
			GraphicsDevice& graphicsDevice = Renderer::GetDevice();

			size_t instanceSize = sizeof(SpriteRenderInstanceData);
			size_t instanceCount = 0;
			for (size_t index = 0; index < queue.mRenderBatchQueue.size(); index++)
			{
				auto& batch = queue.mRenderBatchQueue[index];
				if (batch->mSprite == nullptr) {
					continue;
				}
				instanceCount += batch->mSprite->GetSpriteType() == Sprite::SpriteType_Normal ? 1 : 9;
			}

			GraphicsDevice::GPUAllocation instances = graphicsDevice.AllocateGPU(cmd, instanceCount * instanceSize);
			U32 instancedBatchCount = 0;
			size_t currentInstanceCount = 0;

			// visist render batch queue
			Texture2D* prevTexture = nullptr;
			std::vector<SpriteBatchInstance*> mRenderBatchInstances;
			for (size_t index = 0; index < queue.mRenderBatchQueue.size(); index++)
			{
				auto& batch = queue.mRenderBatchQueue[index];
				Sprite* sprite = batch->mSprite;
				if (sprite == nullptr) {
					continue;
				}

				//U32 currentBatchHash = batch->GetHash();
				//if (currentBatchHash != prevBatchHash)
				Texture2D* currentTexture = batch->GetTexture();
				if (currentTexture != prevTexture)
				{
					prevTexture = currentTexture;
					instancedBatchCount++;

					SpriteBatchInstance* batchInstance = (SpriteBatchInstance*)renderAllocator.Allocate(sizeof(SpriteBatchInstance));
					batchInstance->mTexture = sprite->GetTexture();
					batchInstance->mDataOffset = instances.offset + currentInstanceCount * instanceSize;
					batchInstance->mInstanceCount = 0;

					mRenderBatchInstances.push_back(batchInstance);
				}
		
				// setup instance data
				// TODO : 对9-patchRect单独处理，并且仅用16个顶点绘制
				Rect ninePatchRect = sprite->GetImageParams().mNinePatchRect;
				if (ninePatchRect.mLeft > 0.0f || ninePatchRect.mTop > 0.0f || ninePatchRect.mRight > 0.0f || ninePatchRect.mBottom > 0.0f)
				{
					RenderImage::ImageParams& params = sprite->GetImageParams();
					F32x2 rectSize = {
						params.mSize[0],
						params.mSize[1]
					};

					F32x3 widths  = { ninePatchRect.mLeft, rectSize[0] - (ninePatchRect.mLeft + ninePatchRect.mRight), ninePatchRect.mRight };
					F32x3 heights = { ninePatchRect.mTop,  rectSize[1] - (ninePatchRect.mTop + ninePatchRect.mBottom), ninePatchRect.mBottom };		
					F32x3 texWidths = { ninePatchRect.mLeft, (params.mTexSource[2] - params.mTexSource[0]) - (ninePatchRect.mLeft + ninePatchRect.mRight), ninePatchRect.mRight };
					F32x3 texHeights= { ninePatchRect.mTop,  (params.mTexSource[3] - params.mTexSource[1]) - (ninePatchRect.mTop + ninePatchRect.mBottom), ninePatchRect.mBottom };

					for (int i = 0; i < 9; i++)
					{
						size_t col = i % 3;
						size_t row = i / 3;
						if (widths[col] > 0 && heights[row] > 0)
						{
							F32x2 pos = {
								params.mPos[0] + std::accumulate(widths.begin(),  widths.begin() + col, 0.0f) * params.mScale[0],
								params.mPos[1] + std::accumulate(heights.begin(), heights.begin() + row, 0.0f) * params.mScale[1]
							};
							F32x2 size = { 
								widths[col] * params.mScale[0],
								heights[row] * params.mScale[1]
							};
							F32x4 texRect;
							texRect[0] = params.mTexSource[0] + std::accumulate(texWidths.begin(), texWidths.begin() + col, 0.0f);
							texRect[1] = params.mTexSource[1] + std::accumulate(texHeights.begin(), texHeights.begin() + row, 0.0f);
							texRect[2] = texWidths[col];
							texRect[3] = texHeights[row];

							((SpriteRenderInstanceData*)instances.data)[currentInstanceCount].Setup(sprite, pos, size, texRect);

							mRenderBatchInstances.back()->mInstanceCount++;
							currentInstanceCount++;
						}
					}
				}
				else
				{
					((SpriteRenderInstanceData*)instances.data)[currentInstanceCount].Setup(sprite);

					mRenderBatchInstances.back()->mInstanceCount++;
					currentInstanceCount++;
				}
			}

			if (mRenderBatchInstances.size() > 0)
			{
				graphicsDevice.BindSamplerState(cmd, SHADERSTAGES_PS, *Renderer::GetRenderPreset().GetSamplerState(SamplerStateID_LinearClamp), SAMPLER_LINEAR_CLAMP_SLOT);
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
				graphicsDevice.BindVertexBuffer(cmd, vbs, 0, ARRAYSIZE(vbs), strides, offsets);
				graphicsDevice.BindIndexBuffer(cmd, mIndexBuffer, IndexFormat::INDEX_FORMAT_32BIT, 0);
				graphicsDevice.BindPipelineState(cmd, &state);
				graphicsDevice.BindGPUResource(cmd, SHADERSTAGES_PS, bathInstance->mTexture, TEXTURE_SLOT_0);
				graphicsDevice.DrawIndexedInstanced(cmd, 6, bathInstance->mInstanceCount, 0, 0, 0);
			}

			graphicsDevice.UnAllocateGPU(cmd);
			renderAllocator.Free(instancedBatchCount * sizeof(SpriteBatchInstance));
		}



		///////////////////////////////////////////////////////////////////////////////////

		SpriteRenderBatchQueue mSpriteRenderBatchQueue;
		void RenderSprites(CommandList cmd)
		{
			LinearAllocator& renderAllocator = Renderer::GetRenderAllocator(cmd);
			auto flushQueue = [&]()->void
			{
				//mSpriteRenderBatchQueue.Sort(SpriteRenderBatchQueue::BackToFront);
				ProcessRenderQueue(cmd, mSpriteRenderBatchQueue);
				mSpriteRenderBatchQueue.Clear();

				renderAllocator.Free(mSpriteRenderBatchQueue.mRenderBatchQueue.size() * sizeof(RenderBatch));
			};

			// update sprite buffer
			SpriteCB cb = {};
			DirectX::XMStoreFloat4x4(&cb.gSpriteCameraVP, Renderer::GetScreenProjection());
			Renderer::GetDevice().UpdateBuffer(cmd, mSpriteBuffer, &cb, sizeof(SpriteCB));
			Renderer::GetDevice().BindConstantBuffer(cmd, SHADERSTAGES_VS, mSpriteBuffer, CB_GETSLOT_NAME(SpriteCB));

			// render sprites
			for (Sprite* sprite : mRenderRequestSprites)
			{
				if (sprite != nullptr && sprite->IsVisible())
				{
					SpriteRenderBatch* renderBatch = (SpriteRenderBatch*)renderAllocator.Allocate(sizeof(SpriteRenderBatch));
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

		std::vector<UTF8String> mRenderUT8Strings;
		std::vector<Font::FontParams> mRenderFontParams;
		void RenderTextDrawables(CommandList cmd)
		{
			auto FlushQueue = [&]()->void
			{
				if (mRenderUT8Strings.size() <= 0 || mRenderFontParams.size() <= 0) {
					return;
				}
				Font::Draw(cmd, mRenderUT8Strings, mRenderFontParams);

				mRenderUT8Strings.clear();
				mRenderFontParams.clear();
			};

			TextDrawable* prevTextDrawable = nullptr;
			for (TextDrawable* text : mRenderRequestTexts)
			{
				if (text != nullptr && text->IsVisible() && !text->GetText().Empty()) 
				{
					if (prevTextDrawable != nullptr &&
						prevTextDrawable->GetColor() != text->GetColor())
					{
						FlushQueue();
					}

					mRenderUT8Strings.push_back(text->GetText());
					mRenderFontParams.push_back(text->GetParams());

					prevTextDrawable = text;
				}
			}
			mRenderRequestTexts.clear();
			FlushQueue();
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
		mRenderRequestTexts.clear();
		mRenderLayers.clear();

		mSpriteBuffer.Clear();
		mVertexBuffer.Clear();
		mIndexBuffer.Clear();
		mSpriteBatchPSO.Clear();

		Font::Uninitialize();
	}

	void Render2D(CommandList cmd)
	{
		PROFILER_BEGIN_CPU_GPU_BLOCK(cmd, "Render2D");
		for (auto& layer : mRenderLayers)
		{
			if (layer.mItems.empty()) {
				continue;
			}

			RenderItem2D::RenderItemType prevRenderItemType = RenderItem2D::RenderItemType::RenderItemType_None;
			for (auto& item : layer.mItems)
			{
				RenderItem2D::RenderItemType currentRenderItemType = RenderItem2D::RenderItemType_None;
				if (item.mSprite != nullptr && item.mSprite->IsVisible()) {
					currentRenderItemType = RenderItem2D::RenderItemType_Sprite;
				}
				else if (item.mText != nullptr && item.mText->IsVisible()) 
				{
					currentRenderItemType = RenderItem2D::RenderItemType_Font;
				}
				else if (item.mType == RenderItem2D::RenderItemType_Scissor) {
					currentRenderItemType = RenderItem2D::RenderItemType_Scissor;

				}

				if (currentRenderItemType != RenderItem2D::RenderItemType_None && prevRenderItemType != currentRenderItemType)
				{
					// handle perv render item
					switch (prevRenderItemType)
					{
					case RenderItem2D::RenderItemType_Sprite:
						if (!mRenderRequestSprites.empty()) {
							RenderSprites(cmd);
						}
						break;
					case RenderItem2D::RenderItemType_Font:
						if (!mRenderRequestTexts.empty()) {
							RenderTextDrawables(cmd);
						}
						break;
					default:
						break;
					}

					prevRenderItemType = currentRenderItemType;
				}

				// handle render item
				switch (currentRenderItemType)
				{
				case RenderItem2D::RenderItemType_Sprite:
					mRenderRequestSprites.push_back(item.mSprite);
					break;
				case RenderItem2D::RenderItemType_Font:
					mRenderRequestTexts.push_back(item.mText);
					break;
				case RenderItem2D::RenderItemType_Scissor:
					Renderer::GetDevice().BindScissorRects(cmd, 1, &item.mScissorRect);
					break;
				default:
					break;
				}
			}

			if (!mRenderRequestSprites.empty()) {
				RenderSprites(cmd);
			}
			if (!mRenderRequestTexts.empty()) {
				RenderTextDrawables(cmd);
			}
		}
		PROFILER_END_CPU_GPU_BLOCK();

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