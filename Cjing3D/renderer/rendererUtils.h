#pragma once

#include "renderer\RHI\rhiPipeline.h"

namespace Cjing3D
{
	class GraphicsDevice;

	// ÑÓ³ÙµÄMipÉú³ÉÆ÷
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
}