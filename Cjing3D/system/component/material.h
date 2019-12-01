#pragma once

#include "system\component\componentInclude.h"

namespace Cjing3D {

	class MaterialComponent : public Component
	{
	public:
		MaterialComponent();

		F32x4 mBaseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		F32 mRoughness = 0.2f;
		F32 mMetalness = 0.0f;

		std::string mBaseColorMapName;
		std::string mSurfaceMapName;
		std::string mNormalMapName;

		RhiTexture2DPtr mBaseColorMap = nullptr;
		RhiTexture2DPtr mSurfaceMap = nullptr;
		RhiTexture2DPtr mNormalMap = nullptr;

		inline bool IsCastingShadow()const { return mIsCastingShadow; }
		inline bool IsDirty()const { return mIsDirty; }
		inline void SetIsDirty(bool isDirty) { mIsDirty = isDirty; }

		ShaderMaterial CreateMaterialCB();
		void SetupConstantBuffer(GraphicsDevice& device);
		GPUBuffer& GetConstantBuffer() { return *mConstantBuffer; }
	
	private:
		std::unique_ptr<GPUBuffer> mConstantBuffer = nullptr;

		bool mIsDirty = true;
		bool mIsCastingShadow = false;
	};

}

