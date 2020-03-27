#include "sceneSystem.h"
#include "helper\random.h"

namespace Cjing3D
{
	using namespace ECS;

	void Scene::Serialize(Archive& archive)
	{
		U32 entityCount = 0;
		archive >> entityCount;

		if (entityCount <= 0) {
			return;
		}

		// 依次序列化所有的componentManager
		U32 seed = GENERATE_RANDOM_ID;
		auto allComponentManagers = GetAllComponentManagers();
		std::apply([&archive, &seed](auto&&... componentManager) {
			return (componentManager->Serialize(archive, seed), ...); },
			allComponentManagers
		);

		// reset name mapping
		for (auto entity : mNames.GetEntities())
		{
			auto entityName = mNames.GetComponent(entity);
			mNameEntityMap[entityName->GetName()] = entity;
		}
	}

	void Scene::Unserialize(Archive& archive) const
	{
		U32 entityCount = GetEntityCount();
		archive << entityCount;

		if (entityCount <= 0) {
			return;
		}

		auto allComponentManagers = GetAllComponentManagers();
		std::apply([&archive](auto&&... componentManager) {
			return (componentManager->Unserialize(archive), ...); },
			allComponentManagers
		);
	}
}