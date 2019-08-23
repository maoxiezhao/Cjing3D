#include "system\sceneSystem.h"

namespace Cjing3D
{
	void UpdateSceneTransformSystem(Scene& scene)
	{
		for (size_t index = 0; index < scene.mTransforms.GetCount(); index++)
		{
			auto transform = scene.mTransforms[index];
			transform->Update();
		}
	}
}