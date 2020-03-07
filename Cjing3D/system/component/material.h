#pragma once

#include "system\component\componentInclude.h"

namespace Cjing3D {

	class MaterialComponent : public Component
	{
	public:
		F32x4 mBaseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		F32 mRoughness = 0.2f;
		F32 mMetalness = 0.0f;

		std::string mBaseColorMapName;
		std::string mSurfaceMapName;
		std::string mNormalMapName;

		RhiTexture2DPtr mBaseColorMap = nullptr;
		RhiTexture2DPtr mSurfaceMap = nullptr;
		RhiTexture2DPtr mNormalMap = nullptr;

	public:
		MaterialComponent();

		inline bool IsCastingShadow()const { return mIsCastingShadow; }
		inline bool IsDirty()const { return mIsDirty; }
		inline void SetIsDirty(bool isDirty) { mIsDirty = isDirty; }

		ShaderMaterial CreateMaterialCB();
		void SetupConstantBuffer(GraphicsDevice& device);
		GPUBuffer& GetConstantBuffer() { return *mConstantBuffer; }
	
		virtual void Serialize(Archive& archive, U32 seed = 0);
		virtual void Unserialize(Archive& archive)const;

		bool IsUsingVertexColors()const { return mIsUsingVertexColors; }
		void SetIsUsingVertexColors(bool useVertexColors) { mIsUsingVertexColors = useVertexColors; }

	private:
		std::unique_ptr<GPUBuffer> mConstantBuffer = nullptr;

		bool mIsDirty = true;
		bool mIsUsingVertexColors = false;
		bool mIsCastingShadow = false;

		// TODO£º support custom shader
	};

}

