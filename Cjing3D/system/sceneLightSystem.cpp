#include "system\sceneSystem.h"
#include "renderer\renderer.h"

namespace Cjing3D
{
	void UpdateSceneLightSystem(Scene& scene)
	{
		SystemContext& systemContext = SystemContext::GetSystemContext();
		Renderer& renderer = systemContext.GetSubSystem<Renderer>();
		CameraPtr camera = renderer.GetCamera();

		ECS::ComponentManager<LightComponent > & lights = scene.mLights;
		ECS::ComponentManager<AABBComponent>& lightAABBs = scene.mLightAABBs;
		ECS::ComponentManager<TransformComponent>& transforms = scene.mTransforms;

		for (size_t i = 0; i < lights.GetCount(); i++)
		{
			Entity entity = lights.GetEntityByIndex(i);

			std::shared_ptr<LightComponent> light = lights[i];
			std::shared_ptr<AABBComponent> aabb = lightAABBs[i];
			std::shared_ptr<TransformComponent> transform = transforms.GetComponent(entity);

			if (light == nullptr || aabb == nullptr || transform == nullptr) {
				continue;
			}

			XMMATRIX worldMatrix = DirectX::XMLoadFloat4x4(&transform->GetWorldTransform());
			XMVECTOR S, R, T;
			XMMatrixDecompose(&S, &R, &T, worldMatrix);

			light->mPosition = XMStore<F32x3>(T);
			light->mRotation = XMStore<F32x4>(R);
			light->mDirection = XMStore<F32x3>(XMVector3TransformNormal(XMVectorSet(0, 1, 0, 0), worldMatrix));

			// calculate aabb by light type
			LightComponent::LightType lightType = light->GetLightType();
			switch (lightType)
			{
			case Cjing3D::LightComponent::LightType_Directional:
				aabb->GetAABB().SetFromHalfWidth(XMConvert(camera->GetCameraPos()), XMFLOAT3(1000.0f, 1000.0f, 1000.0f));
				break;
			case Cjing3D::LightComponent::LightType_Point:
				aabb->GetAABB().SetFromHalfWidth(XMConvert(light->mPosition), XMFLOAT3(light->mRange, light->mRange, light->mRange));
				break;
			case Cjing3D::LightComponent::LightType_Spot:
				aabb->GetAABB().SetFromHalfWidth(XMConvert(light->mPosition), XMFLOAT3(light->mRange, light->mRange, light->mRange));
				break;
			default:
				break;
			}
			
		}
	}
}
