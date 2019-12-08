#pragma once

#include "system\component\componentInclude.h"

namespace Cjing3D {

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
		F32x3 mDirection;
		F32x3 mColor = { 1.0f, 1.0f, 1.0f };
		F32 mEnergy = 1.0f;
		F32 mRange = 10.0f;
		bool mIsCastShadow = false;

	public:
		LightComponent();

		inline LightType GetLightType()const { return mLightType; }
		inline void SetLightType(LightType lightType) { mLightType = lightType; }
		inline bool IsCastShadow()const { return mIsCastShadow; }

		ShaderLight CreateShaderLight(XMMATRIX viewMatrix) const;
	};

}

