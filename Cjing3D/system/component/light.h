#pragma once

#include "system\component\componentInclude.h"

namespace Cjing3D {

	LUA_BINDER_REGISTER_CLASS
	class LightComponent : public Component
	{
	public:
		enum LightType
		{
			LightType_Directional = 0,
			LightType_Point,
			LightType_Spot,
			LightType_Count,
		};

		LightType mLightType;
		F32x3 mPosition;
		F32x4 mRotation;
		F32x3 mDirection;
		F32x3 mColor = { 1.0f, 1.0f, 1.0f };
		F32 mEnergy = 1.0f;
		F32 mRange = 10.0f;

		I32 mShadowMapIndex = -1;	// -1表示无阴影贴图
		F32 mShadowBias = 0.0001f;
		bool mIsCastShadow = true;

	public:
		LightComponent();

		LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
		inline LightType GetLightType()const { return mLightType; }
		LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
		inline void SetLightType(LightType lightType) { mLightType = lightType; }
		LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
		inline void SetRange(F32 range) { mRange = range; }
		LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
		inline void SetEnergy(F32 energy) { mEnergy = energy; }
		LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
		inline void SetColor(F32x3 color) { mColor = color; }
		LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
		inline void SetShadowBias(F32 bias) { mShadowBias = bias; }

		inline void SetCastShadow(bool isCastShadow) { mIsCastShadow = isCastShadow; }
		inline bool IsCastShadow()const { return mIsCastShadow; }
		inline void SetShadowMapIndex(I32 index) { mShadowMapIndex = index; }
		inline I32 GetShadowMapIndex()const { return mShadowMapIndex; }

		ShaderLight CreateShaderLight(XMMATRIX viewMatrix) const;

		virtual void Serialize(Archive& archive, U32 seed = 0);
		virtual void Unserialize(Archive& archive)const;
	};

}

