#pragma once

#include "common\common.h"
#include "helper\archive.h"

#include "system\component\genericType.h"
#include "system\component\transform.h"
#include "system\component\camera.h"
#include "system\component\mesh.h"
#include "system\component\object.h"
#include "system\component\material.h"
#include "system\component\light.h"
#include "system\component\terrain.h"
#include "system\component\animation.h"
#include "system\component\weather.h"
#include "system\component\sound.h"

#include <map>
#include <tuple>

namespace Cjing3D {

	// TODO
	using ComponentManagerTypesConst = std::tuple<
		const ECS::ComponentManager<NameComponent>*,
		const ECS::ComponentManager<LayerComponent>*,
		const ECS::ComponentManager<TransformComponent>*,
		const ECS::ComponentManager<HierarchyComponent>*,
		const ECS::ComponentManager<MaterialComponent>*,
		const ECS::ComponentManager<MeshComponent>*,
		const ECS::ComponentManager<ObjectComponent>*,
		const ECS::ComponentManager<AABBComponent>*,
		const ECS::ComponentManager<LightComponent>*,
		const ECS::ComponentManager<AABBComponent>*,
		const ECS::ComponentManager<TerrainComponent>*,
		const ECS::ComponentManager<ArmatureComponent>*,
		const ECS::ComponentManager<AnimationComponent>*,
		const ECS::ComponentManager<WeatherComponent>*,
		const ECS::ComponentManager<SoundComponent>*
	>;

	using ComponentManagerTypes = std::tuple<
		ECS::ComponentManager<NameComponent>*,
		ECS::ComponentManager<LayerComponent>*,
		ECS::ComponentManager<TransformComponent>*,
		ECS::ComponentManager<HierarchyComponent>*,
		ECS::ComponentManager<MaterialComponent>*,
		ECS::ComponentManager<MeshComponent>*,
		ECS::ComponentManager<ObjectComponent>*,
		ECS::ComponentManager<AABBComponent>*,
		ECS::ComponentManager<LightComponent>*,
		ECS::ComponentManager<AABBComponent>*,
		ECS::ComponentManager<TerrainComponent>*,
		ECS::ComponentManager<ArmatureComponent>*,
		ECS::ComponentManager<AnimationComponent>*,
		ECS::ComponentManager<WeatherComponent>*,
		ECS::ComponentManager<SoundComponent>*
	>;

}