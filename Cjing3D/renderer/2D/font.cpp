#include "font.h"
#include "renderer\renderer.h"
#include "renderer\preset\renderPreset.h"
#include "renderer\textureHelper.h"
#include "renderer\RHI\rhiFactory.h"
#include "helper\fileSystem.h"
#include "utils\reckPacker\finders_interface.h"
#include "utils\thread\spinLock.h"
#include "platform\platform.h"
#include "platform\gameWindow.h"
#include "app\engine.h"

#include <DirectXPackedVector.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include "utils\stb_utils\stb_truetype.h"

/**
*  对应的FontGlyph延迟绘制到FontTexture上，绘制时传入对应的位置和纹理坐标的Quad
*
*  TODO: 1. 本地化String支持
*/

namespace Cjing3D {
namespace Font {
	namespace {
		struct FontInfo
		{
			StringID mFileName;
			stbtt_fontinfo mStbFontInfo;
			std::vector<unsigned char> mFontBuffer;

			F32 mFontScaling = 1.0f;	// get from stbtt_ScaleForPixelHeight
			int mAscent = 0;	// 基线到顶部的高度
			int mDescent = 0;   // 基线到底部的高度
			int mLineGap = 0;	// 行间距
		};

		struct FontVertexPosTex
		{
			F32x2 mPos = F32x2(0.0f, 0.0f);
			DirectX::PackedVector::XMHALF2 mTex;
		};

		void LoadFontTTFImpl(const std::string& filePath, std::vector<FontInfo>& loadedFontInfos)
		{
			const StringID filePathStringID = StringID(filePath);
			auto it = std::find_if(loadedFontInfos.begin(), loadedFontInfos.end(), [filePathStringID](FontInfo& fontInfo) {
				return fontInfo.mFileName == filePathStringID;
			});
			if (it == loadedFontInfos.end())
			{
				if (!FileData::IsFileExists(filePath))
				{
					Debug::Warning("Failed to load TTF:" + filePath);
					return;
				}

				FontInfo& fontInfo = loadedFontInfos.emplace_back();
				if (!FileData::ReadFileBytes(filePath, fontInfo.mFontBuffer))
				{
					loadedFontInfos.pop_back();
					Debug::Warning("Failed to load TTF:" + filePath);
					return;
				}
				
				int offset = stbtt_GetFontOffsetForIndex(fontInfo.mFontBuffer.data(), 0);
				if (!stbtt_InitFont(&fontInfo.mStbFontInfo, fontInfo.mFontBuffer.data(), offset))
				{
					loadedFontInfos.pop_back();
					Debug::Warning("Failed to load TTF:" + filePath);
					return;
				}

				fontInfo.mFileName = filePathStringID;
				stbtt_GetFontVMetrics(&fontInfo.mStbFontInfo, &fontInfo.mAscent, &fontInfo.mDescent, &fontInfo.mLineGap);
			}
		}

		// fontCode   0-131071  (0x1FFFF)
		// fontIndex  0-15      (0xF)
		// fontHeight 0-1024    (0x3FF)
		U32 MakeFontGlyphHash(U32 fontCode, U32 fontIndex, U32 height)
		{
			return ((fontCode & 0x1FFFF) << 15) + ((fontIndex & 0xF) << 11) + (height & 0x3FF);
		}
		U32 GetFontCodeFromGlyphHash(U32 hash) 
		{
			return (hash >> 15) & 0x1FFFF;
		}
		U32 GetFontIndexFromGlyphHash(U32 hash)
		{
			return (hash >> 11) & 0xF;
		}
		U32 GetFontHeightFromGlyphHash(U32 hash)
		{
			return hash & 0x3FF;
		}
		struct FontGlyph
		{
			F32 advance = 0.0f;
			F32 x = 0.0f;
			F32 y = 0.0f;
			F32 width = 0.0f;
			F32 height = 0.0f;
			U16 texLeft = 0;
			U16 texRight = 0;
			U16 texTop = 0;
			U16 texBottom = 0;
		};
	}

	namespace {
	///////////////////////////////////////////////////////////////////////////////////
		bool mIsInitialized = false;
		std::vector<FontInfo> mLoadedFontInfos;
		SpinLock mFontLock;

		// render
		GPUBuffer mIndexBuffer;
		GPUBuffer mFontBuffer;
		PipelineState mFontPSO;
		Texture2D mFontGlyphsTexture;

		// font glyphs
		I32 currentFontDPI = 96;
		const U32 GlyphsRectBorderPadding = 1;
		std::unordered_map<U32, FontGlyph> mFontGlyphsLookup;
		std::vector<U32> mPendingUpdateFontGlyphs;
		std::unordered_map<U32, U32> mFontGlyphsRectLookup;
		std::vector<rectpack2D::rect_xywh> mFontGlyphsRects;

	///////////////////////////////////////////////////////////////////////////////////
		void InitializeImpl()
		{
			RenderPreset& renderPreset = Renderer::GetRenderPreset();
			GraphicsDevice& device = Renderer::GetDevice();

			// create font constant buffer
			const HRESULT result = CreateDynamicConstantBuffer(device, mFontBuffer, sizeof(FontCB));
			Debug::ThrowIfFailed(result, "failed to create font constant buffer:%08x", result);
			device.SetResourceName(mFontBuffer, "FontCB");

			// create index buffer
			std::vector<U16> indicesData(6 * MaxFontRenderLength);
			for (int i = 0; i < MaxFontRenderLength * 4; i += 4)
			{
				indicesData[(size_t)i / 4 * 6 + 0] = i + 0;
				indicesData[(size_t)i / 4 * 6 + 1] = i + 1;
				indicesData[(size_t)i / 4 * 6 + 2] = i + 2;
				indicesData[(size_t)i / 4 * 6 + 3] = i + 1;
				indicesData[(size_t)i / 4 * 6 + 4] = i + 3;
				indicesData[(size_t)i / 4 * 6 + 5] = i + 2;
			}
			const auto bufferResult = CreateStaticIndexBuffer(device, mIndexBuffer, indicesData);
			Debug::ThrowIfFailed(bufferResult, "Failed to create index buffer:%08x", bufferResult);

			// create pipelinestate
			VertexLayoutDesc posLayout[] =
			{
				{ "POSITION",  0u, FORMAT_R32G32_FLOAT, 0u, APPEND_ALIGNED_ELEMENT,  INPUT_PER_VERTEX_DATA , 0u },
				{ "TEXCOORD",  0u, FORMAT_R16G16_FLOAT, 0u, APPEND_ALIGNED_ELEMENT,  INPUT_PER_VERTEX_DATA , 0u },
			};
			auto vsinfo = Renderer::LoadVertexShaderInfo("fontVS.cso", posLayout, ARRAYSIZE(posLayout));
			PipelineStateDesc desc = {};
			desc.mInputLayout = vsinfo.mInputLayout;
			desc.mVertexShader = vsinfo.mVertexShader;
			desc.mPixelShader = Renderer::LoadShader(SHADERSTAGES_PS, "fontPS.cso");
			desc.mPrimitiveTopology = TRIANGLELIST;
			desc.mBlendState = renderPreset.GetBlendState(BlendStateID_PreMultiplied);
			desc.mDepthStencilState = renderPreset.GetDepthStencilState(DepthStencilStateID_DepthNone);
			desc.mRasterizerState = renderPreset.GetRasterizerState(RasterizerStateID_Image);
			device.CreatePipelineState(desc, mFontPSO);
		}

		///////////////////////////////////////////////////////////////////////////////////

		template<typename CharT>
		F32 GetTextWidth(const CharT* text, size_t length, const FontParams& params, std::vector<F32>* widths = nullptr)
		{
			if (length <= 0) {
				return 0.0f;
			}

			F32 spaceWidth = (F32(params.mFontSize) * params.mScale * 0.25f);
			F32 tabWidth = 4.0f * spaceWidth;

			U32 prevCode = 0;
			F32 currentWidth = 0;
			F32 maxWidth = 0;
			size_t i = 0;
			while (text[i] != 0 && i < length)
			{
				U32 code = (U32)text[i++];
				if (code <= 0) {
					continue;
				}

				U32 glyphHash = MakeFontGlyphHash(code, params.mFontStyleIndex, params.mFontSize);
				if (mFontGlyphsLookup.count(glyphHash) == 0) 
				{
					FontInfo& fontInfo = mLoadedFontInfos[params.mFontStyleIndex];
					if (stbtt_FindGlyphIndex(&fontInfo.mStbFontInfo, code) == 0) {
						continue;
					}

					F32 fontScaling = stbtt_ScaleForPixelHeight(&fontInfo.mStbFontInfo, (F32)params.mFontSize);
					F32 kernAdvance = 0.0f;
					if (prevCode != 0)
					{
						int kern = stbtt_GetCodepointKernAdvance(&fontInfo.mStbFontInfo, prevCode, code);
						kernAdvance += kern * fontScaling * params.mScale;
					}
					prevCode = code;

					I32 advanceWidth, lsb = 0;
					stbtt_GetCodepointHMetrics(&fontInfo.mStbFontInfo, code, &advanceWidth, &lsb);

					currentWidth += kernAdvance + (F32)advanceWidth * fontScaling * params.mScale + params.mColSpacing;
					maxWidth = std::max(maxWidth, currentWidth);

					if (widths != nullptr) {
						widths->push_back(currentWidth);
					}

					continue;
				}

				if (code == '\n')
				{
					currentWidth = 0.0f;
					prevCode = 0;
				}
				else if (code == ' ')
				{
					currentWidth += spaceWidth;
					prevCode = 0;
				}
				else if (code == '\t')
				{
					currentWidth += tabWidth;
					prevCode = 0;
				}
				else
				{
					const FontGlyph& glyph = mFontGlyphsLookup.at(glyphHash);
					F32 kernAdvance = 0.0f;
					if (prevCode != 0)
					{
						auto& fontInfo = mLoadedFontInfos[params.mFontStyleIndex];
						int kern = stbtt_GetCodepointKernAdvance(&fontInfo.mStbFontInfo, prevCode, code);
						kernAdvance += kern * fontInfo.mFontScaling * params.mScale;
					}
					prevCode = code;
					currentWidth += kernAdvance + glyph.advance * params.mScale + params.mColSpacing;
				}

				if (widths != nullptr) {
					widths->push_back(currentWidth);
				}

				maxWidth = std::max(maxWidth, currentWidth);
			}
			return maxWidth;
		}

		template<typename CharT>
		F32 GetTextHeight(const CharT* text, size_t length, const FontParams& params)
		{
			if (length <= 0) {
				return 0.0f;
			}

			F32 lineHeight = ((F32(params.mFontSize) + params.mLineSpacing) * params.mScale);
			F32 currentHeight = lineHeight;
			size_t i = 0;
			while (text[i] != 0 && i < length)
			{
				U32 code = (U32)text[i++];
				if (code <= 0) {
					continue;
				}
				if (code == '\n') {
					currentHeight += lineHeight;
				}
			}
			return currentHeight;
		}
		
		void PackGlyphsTexture()
		{
			using spacesType = rectpack2D::empty_spaces<false, rectpack2D::default_empty_spaces>;
			using rectType = rectpack2D::output_rect_t<spacesType>;

			// 根据传入的Rects，得到besting rect2D packing, 并写入到texture中
			auto report_successful = [](rectType&) {
				return rectpack2D::callback_result::CONTINUE_PACKING;
			};
			auto report_unsuccessful = [](rectType&) {
				return rectpack2D::callback_result::ABORT_PACKING;
			};
			const auto resultSize = rectpack2D::find_best_packing<spacesType>(
				mFontGlyphsRects,
				rectpack2D::make_finder_input(
					4096,
					1,
					report_successful,
					report_unsuccessful,
					rectpack2D::flipping_option::DISABLED
				)
			);
			if (resultSize.area() > 0)
			{
				const U32 texWidth = resultSize.w;
				const U32 texHeight = resultSize.h;
				const F32 invTexWidth = 1.0f / (F32)texWidth;
				const F32 invTexHeight = 1.0f / (F32)texHeight;

				std::vector<U8> glyphsBuffer((size_t)texWidth * (size_t)texHeight * sizeof(U8));
				std::fill(glyphsBuffer.begin(), glyphsBuffer.end(), 0);

				for (auto it : mFontGlyphsRectLookup)
				{
					const U32 hash = it.first;
					U32 fontCode = GetFontCodeFromGlyphHash(hash);
					U32 fontIndex = GetFontIndexFromGlyphHash(hash);
					U32 fontHeight = GetFontHeightFromGlyphHash(hash);
					FontInfo& fontInfo = mLoadedFontInfos[fontIndex];

					FontGlyph& fontGlyph = mFontGlyphsLookup[hash];
					rectpack2D::rect_xywh fontRect = mFontGlyphsRects[it.second];

					// remove border padding
					fontRect.x += GlyphsRectBorderPadding;
					fontRect.y += GlyphsRectBorderPadding;
					fontRect.w -= GlyphsRectBorderPadding * 2;
					fontRect.h -= GlyphsRectBorderPadding * 2;

					// 将glyph bitmap数据集直接写入到buffer中
					float fontScaling = stbtt_ScaleForPixelHeight(&fontInfo.mStbFontInfo, (F32)fontHeight);
					int offset = fontRect.x + fontRect.y * texWidth;
					stbtt_MakeCodepointBitmap(&fontInfo.mStbFontInfo, glyphsBuffer.data() + offset, fontRect.w, fontRect.h, texWidth, fontScaling, fontScaling, fontCode);
				
					// 计算并保存当前glyph的texCoords
					F32 texLeft = (F32)fontRect.x * invTexWidth;
					F32 texRight = (F32)(fontRect.x + fontRect.w) * invTexWidth;
					F32 texTop = (F32)fontRect.y * invTexHeight;
					F32 texBottom = (F32)(fontRect.y + fontRect.h) * invTexHeight;

					fontGlyph.texLeft = DirectX::PackedVector::XMConvertFloatToHalf(texLeft);
					fontGlyph.texRight = DirectX::PackedVector::XMConvertFloatToHalf(texRight);
					fontGlyph.texTop = DirectX::PackedVector::XMConvertFloatToHalf(texTop);
					fontGlyph.texBottom = DirectX::PackedVector::XMConvertFloatToHalf(texBottom);
				}

				TextureHelper::CreateTexture(mFontGlyphsTexture, glyphsBuffer.data(), texWidth, texHeight, FORMAT_R8_UNORM);
			}
		}

		void UpdataPendingFontGlyphs()
		{
			GuardSpinLock lock(mFontLock);

			// 检查当前dpi，如果dpi发生变化需要重新生成字体纹理
			I32 dpi = GetGlobalContext().GetEngine()->GetGameWindow()->GetDPI();
			if (dpi != currentFontDPI)
			{
				currentFontDPI = dpi;

				for (auto it : mFontGlyphsRectLookup)
				{
					const U32 hash = it.first;
					mPendingUpdateFontGlyphs.push_back(hash);
				}
				mFontGlyphsLookup.clear();
				mFontGlyphsRectLookup.clear();
				mFontGlyphsRects.clear();
			}

			if (mPendingUpdateFontGlyphs.empty()) {
				return;
			}

			// 1.首先为每个pendingGlyhps构建RECT,用于下一步rectPacking
			for (const auto& hash : mPendingUpdateFontGlyphs)
			{
				U32 fontCode = GetFontCodeFromGlyphHash(hash);
				U32 fontIndex = GetFontIndexFromGlyphHash(hash);
				U32 fontHeight = GetFontHeightFromGlyphHash(hash);
				FontInfo& fontInfo = mLoadedFontInfos[fontIndex];

				if (stbtt_FindGlyphIndex(&fontInfo.mStbFontInfo, fontCode) == 0) {
					continue;
				}

				float fontScaling = stbtt_ScaleForPixelHeight(&fontInfo.mStbFontInfo, (F32)fontHeight);
				fontInfo.mFontScaling = fontScaling;
				int top, left, right, bottom;
				stbtt_GetCodepointBitmapBox(&fontInfo.mStbFontInfo, fontCode, fontScaling, fontScaling, &left, &top, &right, &bottom);

				if ((bottom - top) <= 0.0f || (right - left) <= 0.0f) {
					continue;
				}

				I32 advanceWidth, lsb = 0;
				stbtt_GetCodepointHMetrics(&fontInfo.mStbFontInfo, fontCode, &advanceWidth, &lsb);

				FontGlyph& fontGlyph = mFontGlyphsLookup[hash];
				fontGlyph.x = (F32)left;
				fontGlyph.y = (F32)top + (fontInfo.mAscent) * fontScaling; // 对齐基线
				fontGlyph.width = (F32)(right - left);
				fontGlyph.height = (F32)(bottom - top);
				fontGlyph.advance = (F32)advanceWidth * fontScaling;

				// create rect and add padding to the rect
				bottom += GlyphsRectBorderPadding * 2;
				right += GlyphsRectBorderPadding * 2;
				mFontGlyphsRects.emplace_back(rectpack2D::rect_xywh(left, top, right - left, bottom - top));
				mFontGlyphsRectLookup[hash] = (mFontGlyphsRects.size() - 1);
			}
			mPendingUpdateFontGlyphs.clear();

			// 2.为所有mFontGlyphsRectLookup的rect，pack为一个RECT，写入到texture
			PackGlyphsTexture();
		}

		template<typename CharT>
		U32 GetFontQuadDataByText(const CharT* text, size_t length, const FontParams& params, FontVertexPosTex* quadBuffer)
		{
			U32 quadCount = 0;
			if (length <= 0) {
				return quadCount;
			}

			F32 lineHeight = ((F32(params.mFontSize) + params.mLineSpacing) * params.mScale);
			F32 spaceWidth = (F32(params.mFontSize) * params.mScale * 0.25f);
			F32 tabWidth = 4.0f * spaceWidth;

			F32 currentPosY = 0;
			F32 currentPosX = 0;
			U32 prevCode = 0;

			F32 boundingWidth = params.mBoundingSize[0];
			F32 boundingHeight = params.mBoundingSize[1];
			auto checkBoundingWidth = [&](F32 width)
			{
				if (boundingWidth > 0.0f && currentPosX + width > boundingWidth)
				{
					currentPosY += lineHeight;
					currentPosX = 0;
					prevCode = 0;
					return false;
				}
				return true;
			};


			size_t i = 0;
			while (text[i] != 0 && i < length)
			{
				U32 code = (U32)text[i++];
				if (code <= 0) {
					continue;
				}

				if (code == '\n')
				{
					if (boundingHeight > 0.0f && currentPosY + lineHeight > boundingHeight) {
						return quadCount;
					}

					currentPosY += lineHeight;
					currentPosX = 0;
					prevCode = 0;
				}
				else if (code == ' ')
				{
					if (checkBoundingWidth(spaceWidth))
					{
						currentPosX += spaceWidth;
						prevCode = 0;
					}
				}
				else if (code == '\t')
				{
					if (checkBoundingWidth(tabWidth))
					{
						currentPosX += tabWidth;
						prevCode = 0;
					}
				}
				else
				{
					U32 glyphHash = MakeFontGlyphHash(code, params.mFontStyleIndex, params.mFontSize);
					if (mFontGlyphsLookup.count(glyphHash) == 0)
					{
						// 对于当前未添加的Glyph，则延迟添加渲染
						{
							GuardSpinLock lock(mFontLock);
							mPendingUpdateFontGlyphs.push_back(glyphHash);
						}
						continue;
					}

					F32 kernAdvance = 0.0f;
					if (prevCode != 0)
					{
						auto& fontInfo = mLoadedFontInfos[params.mFontStyleIndex];
						int kern = stbtt_GetCodepointKernAdvance(&fontInfo.mStbFontInfo, prevCode, code);
						kernAdvance += kern * fontInfo.mFontScaling * params.mScale;
					}
					prevCode = code;

					// check bounding
					const FontGlyph& glyph = mFontGlyphsLookup.at(glyphHash);
					const F32 x = glyph.x * params.mScale;
					const F32 y = glyph.y * params.mScale;
					const F32 width = glyph.width * params.mScale;
					const F32 height = glyph.height * params.mScale;
					
					F32 glyphAdvance = glyph.advance * params.mScale;
					if (!checkBoundingWidth(kernAdvance + glyphAdvance)) {
						continue;
					}
					if (boundingHeight > 0.0f && currentPosY + y + height > boundingHeight) {
						return quadCount;
					}

					currentPosX += kernAdvance;

					// get glyph rect
					F32 left   = currentPosX + x + params.mPos[0];
					F32 top    = currentPosY + y + params.mPos[1];
					F32 right  = left + width;
					F32 bottom = top  + height;

					size_t index = (size_t)quadCount * 4;
					quadBuffer[index + 0].mPos = { left,  top };
					quadBuffer[index + 1].mPos = { right, top };
					quadBuffer[index + 2].mPos = { left,  bottom };
					quadBuffer[index + 3].mPos = { right, bottom };

					quadBuffer[index + 0].mTex = { glyph.texLeft,  glyph.texTop };
					quadBuffer[index + 1].mTex = { glyph.texRight, glyph.texTop };
					quadBuffer[index + 2].mTex = { glyph.texLeft,  glyph.texBottom };
					quadBuffer[index + 3].mTex = { glyph.texRight, glyph.texBottom };

					currentPosX += glyphAdvance + params.mColSpacing;
					quadCount++;
				}
			}

			return quadCount;
		}

		template<typename CharT>
		void DrawImpl(const CharT* text, size_t length, const FontParams& params)
		{
			GraphicsDevice& device = Renderer::GetDevice();
			GraphicsDevice::GPUAllocation gpuMem = device.AllocateGPU(sizeof(FontVertexPosTex) * length * 4);
			if (!gpuMem.IsValid()) {
				return;
			}

			FontParams currentFontParams = params;
			// text align Horizonal
			switch (currentFontParams.mTextAlignH)
			{
			case TextAlignH_Center:
				currentFontParams.mPos[0] -= GetTextWidth(text, length, currentFontParams) / 2.0f;
				break;
			case TextAlignH_Right:
				currentFontParams.mPos[0] -= GetTextWidth(text, length, currentFontParams);
				break;
			default:
				break;
			}
			// text align vertical
			switch (currentFontParams.mTextAlignV)
			{
			case TextAlignV_Center:
				currentFontParams.mPos[1] -= GetTextHeight(text, length, currentFontParams) / 2.0f;
				break;
			case TextAlignV_Bottom:
				currentFontParams.mPos[1] -= GetTextHeight(text, length, currentFontParams);
				break;
			default:
				break;
			}

			U32 quadCount = GetFontQuadDataByText(text, length, currentFontParams, (FontVertexPosTex*)gpuMem.data);
			if (quadCount <= 0)
			{
				device.UnAllocateGPU();
				UpdataPendingFontGlyphs();
				return;
			}

			device.BeginEvent("RenderFontInst");
			device.BindPipelineState(mFontPSO);
			device.BindSamplerState(SHADERSTAGES_PS, *Renderer::GetRenderPreset().GetSamplerState(SamplerStateID_Font), SAMPLER_SLOT_0);
			device.BindGPUResource(SHADERSTAGES_PS, &mFontGlyphsTexture, TEXTURE_SLOT_FONT);
			device.BindConstantBuffer(SHADERSTAGES_VS, mFontBuffer, CB_GETSLOT_NAME(FontCB));
			device.BindConstantBuffer(SHADERSTAGES_PS, mFontBuffer, CB_GETSLOT_NAME(FontCB));

			FontCB cb = {};
			cb.gFontColor = XMConvert(currentFontParams.mColor.ToFloat4());
			XMStoreFloat4x4(&cb.gFontTransform, 
				XMMatrixTranslation(currentFontParams.mPos[0], currentFontParams.mPos[1], 0.0f) * 
				Renderer::GetScreenProjection());
			device.UpdateBuffer(mFontBuffer, &cb, sizeof(FontCB));

			GPUBuffer* vbs[] = {
				gpuMem.buffer,
			};
			U32 strides[] = {
				sizeof(FontVertexPosTex),
			};
			U32 offsets[] = {
				gpuMem.offset,
			};
			device.BindVertexBuffer(vbs, 0, ARRAYSIZE(vbs), strides, offsets);
			device.BindIndexBuffer(mIndexBuffer, IndexFormat::INDEX_FORMAT_16BIT, 0);
			device.DrawIndexed(6 * quadCount, 0);
			device.EndEvent();
			device.UnAllocateGPU();

			UpdataPendingFontGlyphs();
		}

		template<typename CharT>
		void DrawImpl(const std::vector<std::vector<CharT>>& texts, const FontParams* params, size_t count, size_t totalLength)
		{
			GraphicsDevice& device = Renderer::GetDevice();
			GraphicsDevice::GPUAllocation gpuMem = device.AllocateGPU(sizeof(FontVertexPosTex) * totalLength * 4);
			if (!gpuMem.IsValid()) {
				return;
			}
	
			size_t totalQuadCount = 0;
			for (int i = 0; i < count; i++)
			{
				FontParams currentFontParams = params[i];
				const CharT* text = texts[i].data();
				size_t length = texts[i].size();

				// text align Horizonal
				switch (currentFontParams.mTextAlignH)
				{
				case TextAlignH_Center:
					currentFontParams.mPos[0] -= GetTextWidth(text, length, currentFontParams) / 2.0f;
					break;
				case TextAlignH_Right:
					currentFontParams.mPos[0] -= GetTextWidth(text, length, currentFontParams);
					break;
				default:
					break;
				}
				// text align vertical
				switch (currentFontParams.mTextAlignV)
				{
				case TextAlignV_Center:
					currentFontParams.mPos[1] -= GetTextHeight(text, length, currentFontParams) / 2.0f;
					break;
				case TextAlignV_Bottom:
					currentFontParams.mPos[1] -= GetTextHeight(text, length, currentFontParams);
					break;
				default:
					break;
				}

				FontVertexPosTex* address = (FontVertexPosTex*)(gpuMem.data) + totalQuadCount * 4;
				totalQuadCount += (size_t)GetFontQuadDataByText(text, length, currentFontParams, address);
			}

			if (totalQuadCount <= 0)
			{
				device.UnAllocateGPU();
				UpdataPendingFontGlyphs();
				return;
			}

			FontParams currentFontParams = params[0];
			device.BindPipelineState(mFontPSO);
			device.BindSamplerState(SHADERSTAGES_PS, *Renderer::GetRenderPreset().GetSamplerState(SamplerStateID_Font), SAMPLER_SLOT_0);
			device.BindGPUResource(SHADERSTAGES_PS, &mFontGlyphsTexture, TEXTURE_SLOT_FONT);
			device.BindConstantBuffer(SHADERSTAGES_VS, mFontBuffer, CB_GETSLOT_NAME(FontCB));
			device.BindConstantBuffer(SHADERSTAGES_PS, mFontBuffer, CB_GETSLOT_NAME(FontCB));

			FontCB cb = {};
			cb.gFontColor = XMConvert(currentFontParams.mColor.ToFloat4());
			XMStoreFloat4x4(&cb.gFontTransform, Renderer::GetScreenProjection());
			device.UpdateBuffer(mFontBuffer, &cb, sizeof(FontCB));

			GPUBuffer* vbs[] = {
				gpuMem.buffer,
			};
			U32 strides[] = {
				sizeof(FontVertexPosTex),
			};
			U32 offsets[] = {
				gpuMem.offset,
			};
			device.BindVertexBuffer(vbs, 0, ARRAYSIZE(vbs), strides, offsets);
			device.BindIndexBuffer(mIndexBuffer, IndexFormat::INDEX_FORMAT_16BIT, 0);
			device.DrawIndexed(6 * totalQuadCount, 0);
			device.UnAllocateGPU();

			UpdataPendingFontGlyphs();
		}
	}

	void LoadFontTTF(const std::string& ttfFile)
	{
		LoadFontTTFImpl(ttfFile, mLoadedFontInfos);
	}

	void ClearCurrentGlyphs()
	{
		mFontGlyphsLookup.clear();
		mFontGlyphsRectLookup.clear();
		mFontGlyphsRects.clear();
	}

	void PrepareFonts(const UTF8String& text)
	{
		// TODO
	}

	I32 GetFontStyleIndexByName(const std::string& ttfFile)
	{
		const StringID filePathStringID = StringID(ttfFile);
		auto it = std::find_if(mLoadedFontInfos.begin(), mLoadedFontInfos.end(), [filePathStringID](FontInfo& fontInfo) {
			return fontInfo.mFileName == filePathStringID;
			});
		if (it == mLoadedFontInfos.end()) {
			return -1;
		}

		return (I32)(it - mLoadedFontInfos.begin());
	}

	F32 GetTextWidth(const UTF8String& text, const FontParams& params)
	{
		if (!mIsInitialized) {
			return 0.0f;
		}
		auto codepoints = text.GetCodePoints();
		return GetTextWidth(codepoints.data(), codepoints.size(), params);
	}

	F32 GetTextHeight(const UTF8String& text, const FontParams& params)
	{
		if (!mIsInitialized) {
			return 0.0f;
		}
		auto codepoints = text.GetCodePoints();
		return GetTextHeight(codepoints.data(), codepoints.size(), params);
	}

	std::vector<F32> GetTextWidths(const UTF8String& text, const FontParams& params)
	{
		if (!mIsInitialized) {
			std::vector<F32>();
		}

		std::vector<F32> ret;
		auto codepoints = text.GetCodePoints();
		GetTextWidth(codepoints.data(), codepoints.size(), params, &ret);
		return ret;
	}

	void Draw(const std::string& text, const FontParams& params)
	{
		if (!mIsInitialized) {
			return;
		}
		DrawImpl(text.c_str(), text.length(), params);
	}

	void Draw(const std::wstring& text, const FontParams& params)
	{
		if (!mIsInitialized) {
			return;
		}
		DrawImpl(text.c_str(), text.length(), params);
	}

	void Draw(const UTF8String& text, const FontParams& params)
	{
		if (!mIsInitialized) {
			return;
		}
		auto codepoints = text.GetCodePoints();
		DrawImpl(codepoints.data(), codepoints.size(), params);
	}

	void Draw(const std::vector<UTF8String>& text, const std::vector<FontParams>& params)
	{
		// 合并渲染需要保证所有字体的渲染参数一致（字体颜色）
		if (!mIsInitialized) {
			return;
		}

		Debug::CheckAssertion(text.size() == params.size());
		U32 totalCount = text.size();
		U32 totalLength = 0;

		U32 lastEndIndex = 0;
		auto FlushFunc = [&](U32 currentIndex) 
		{
			if (lastEndIndex > currentIndex) {
				return;
			}

			std::vector<std::vector<I32>> codePoints;
			for (int i = lastEndIndex; i < currentIndex; i++)
			{
				auto codepoints = text[i].GetCodePoints();
				codePoints.push_back(codepoints);
			}

			DrawImpl(codePoints, params.data() + lastEndIndex, currentIndex - lastEndIndex, totalLength);
			lastEndIndex = currentIndex;
		};

		for (int i = 0; i < totalCount; i++)
		{
			if (text[i].Length() + totalLength > MaxFontRenderLength)
			{
				FlushFunc(i);
				totalLength = 0;
			}
			totalLength += text[i].Length();
		}

		if (totalLength > 0) {
			FlushFunc(totalCount);
		}
	}

	void Initialize()
	{
		if (mIsInitialized) {
			return;
		}
		mIsInitialized = true;
		InitializeImpl();
		Logger::Info("Font system initialized");
	}

	void Uninitialize()
	{
		if (!mIsInitialized) {
			return;
		}
		mIsInitialized = false;

		mFontGlyphsTexture.Clear();
		mFontPSO.Clear();
		mIndexBuffer.Clear();
		mFontBuffer.Clear();
		Logger::Info("Font system uninitialized");
	}
}
}