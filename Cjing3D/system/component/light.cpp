#include "light.h"
#include "helper\enumInfo.h"

namespace Cjing3D
{
	LightComponent::LightComponent() :
		Component(ComponentType_Light)
	{
	}

	ShaderLight LightComponent::CreateShaderLight(XMMATRIX viewMatrix) const
	{
		ShaderLight shaderLight;
		shaderLight.worldPosition = XMConvert(mPosition);
		shaderLight.type = static_cast<uint>(mLightType);
		shaderLight.range = mRange;
		shaderLight.energy = mEnergy;
		shaderLight.color = XMConvert(F32x4(mColor, 1.0f));

		XMFLOAT3 viewPosition;
		XMStoreFloat3(&viewPosition, XMVector3TransformCoord(XMLoadFloat3(&shaderLight.worldPosition), viewMatrix));
		shaderLight.viewPosition = viewPosition;

		switch (mLightType)
		{
		case Cjing3D::LightComponent::LightType_Directional:
			shaderLight.direction = XMConvert(mDirection);
			break;
		case Cjing3D::LightComponent::LightType_Point:
			break;
		case Cjing3D::LightComponent::LightType_Spot:
			break;
		case Cjing3D::LightComponent::LightType_Count:
			break;
		default:
			break;
		}

		return shaderLight;
	}

	void LightComponent::Serialize(Archive& archive, U32 seed)
	{
		archive >> mLightType;
		archive >> mPosition;
		archive >> mDirection;
		archive >> mColor;
		archive >> mEnergy;
		archive >> mRange;
	}

	void LightComponent::Unserialize(Archive& archive) const
	{
		archive << mLightType;
		archive << mPosition;
		archive << mDirection;
		archive << mColor;
		archive << mEnergy;
		archive << mRange;
	}
}