#pragma once

#include "core\gameComponent.h"
#include "game\gameContext.h"
#include "game\gameObject.h"
#include "gui\guiStage.h"
#include "game\map\gameMap.h"

namespace CjingGame
{
	enum EditorMode
	{
		EditorMode_Ground,
		EditorMode_Wall,
	};

	enum CameraMode
	{
		CameraMode_Free,
		CameraMode_TraceCursor
	};

	class EditorCursor : public GameObject
	{
	public:
		EditorCursor(const I32x3& localPos);
		~EditorCursor();

		void (LoadCursor)(const std::string& path);
		MapPartPosition GetMapPartPos()const;

	private:
		I32x3 mLocalPos;
	};

}