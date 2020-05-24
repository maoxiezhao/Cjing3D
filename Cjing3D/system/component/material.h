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

		Texture2DPtr mBaseColorMap = nullptr;
		Texture2DPtr mSurfaceMap = nullptr;
		Texture2DPtr mNormalMap = nullptr;

	public:
		MaterialComponent();

		inline void SetBlendMode(BlendType blendType) { mBlendType = blendType; }
		inline BlendType GetBlendMode()const { return mBlendType; }
		inline bool IsTransparent()const { return mBlendType != BlendType_Opaque; }
		inline bool IsCastingShadow()const { return mIsCastingShadow; }
		inline void SetCastShadow(bool castingShadow) { mIsCastingShadow = castingShadow; }
		inline bool IsUsingVertexColors()const { return mIsUsingVertexColors; }
		inline void SetIsUsingVertexColors(bool useVertexColors) { mIsUsingVertexColors = useVertexColors; }
		inline F32  GetAlphaCutRef()const { return mAlphaCutRef; }
		inline void SetAlphaCutRef(F32 alpha) { mAlphaCutRef = alpha; }

		inline bool IsNeedAlphaTest()const { return mAlphaCutRef < (1.0f - 0.0001); }

		inline bool IsDirty()const { return mIsDirty; }
		inline void SetIsDirty(bool isDirty) { mIsDirty = isDirty; }

		ShaderMaterial CreateMaterialCB();
		void SetupConstantBuffer(GraphicsDevice& device);
		GPUBuffer& GetConstantBuffer() { return mConstantBuffer; }

		virtual void Serialize(Archive& archive, U32 seed = 0);
		virtual void Unserialize(Archive& archive)const;

	private:
		GPUBuffer mConstantBuffer;

		bool mIsDirty = true;
		bool mIsUsingVertexColors = false;
		bool mIsCastingShadow = false;
		BlendType mBlendType = BlendType_Opaque;
		F32 mAlphaCutRef = 1.0f;

		// TODO£º support custom shader
	};

}

