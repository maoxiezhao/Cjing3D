#include "levelEditorUtils.h"

namespace CjingGame
{
	EditorCursor::EditorCursor(const I32x3& localPos) :
		GameObject("EditorCursor"),
		mLocalPos(localPos)
	{
	}

	EditorCursor::~EditorCursor()
	{
	}

	void EditorCursor::UpdatePosition(GameMap* gameMap)
	{
		if (gameMap == nullptr)
		{
			SetPosition({ 0.0f, 0.0f, 0.0f });
			return;
		}

		F32x3 globalPos = gameMap->TransformLocalPosToGlobal(mLocalPos);
		SetPosition(globalPos);
	}

	void EditorCursor::LoadCursor(const std::string& path)
	{
		SetMeshFromModel(path);

		auto object = GetObjectComponent();
		if (object == nullptr) {
			return;
		}

		Scene& scene = Scene::GetScene();
		auto mesh = scene.mMeshes.GetComponent(object->mMeshID);
		if (mesh == nullptr) {
			return;
		}

		Entity materialEntity = INVALID_ENTITY;
		if (mesh->mSubsets.size() > 0) {
			// 暂时取第一个subset的materialID
			materialEntity = mesh->mSubsets[0].mMaterialID;
		}

		if (materialEntity != ECS::INVALID_ENTITY)
		{
			auto material = scene.mMaterials.GetComponent(materialEntity);
			if (material != nullptr) {
				material->SetBlendMode(BlendType_Alpha);
			}
		}
	}

	MapPartPosition EditorCursor::GetMapPartPos() const
	{
		return MapPartPosition::TransformFromLocalPos(mLocalPos);
	}
}