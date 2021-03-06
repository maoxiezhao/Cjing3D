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

		TextureResourcePtr mBaseColorMap = nullptr;
		TextureResourcePtr mSurfaceMap = nullptr;
		TextureResourcePtr mNormalMap = nullptr;

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

		bool LoadBaseColorMap(const std::string& filePath);
		bool LoadNormalMap(const std::string& filePath);
		bool LoadSurfaceMap(const std::string& filePath);
		
		const Texture2D* GetBaseColorMap()const;
		const Texture2D* GetNormalMap()const;
		const Texture2D* GetSurfaceMap()const;

		Texture2D* GetBaseColorMapPtr()const;
		Texture2D* GetNormalMapPtr()const;
		Texture2D* GetSurfaceMapPtr()const;

		void SetCustomShader(const StringID& id) { mCustomShader = id; }
		StringID GetCustomShader()const { return mCustomShader; }

		virtual void Serialize(Archive& archive, U32 seed = 0);
		virtual void Unserialize(Archive& archive)const;

	private:
		GPUBuffer mConstantBuffer;

		bool mIsDirty = true;
		bool mIsUsingVertexColors = false;
		bool mIsCastingShadow = true;
		BlendType mBlendType = BlendType_Opaque;
		F32 mAlphaCutRef = 1.0f;

		// TODO�� support custom shader
		StringID mCustomShader = StringID::EMPTY;
	};

}

