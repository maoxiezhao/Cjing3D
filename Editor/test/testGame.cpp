#include "testGame.h"
#include "editor\guiEditor.h"
#include "core\eventSystem.h"
#include "renderer\renderer.h"
#include "renderer\paths\renderPath_tiledForward.h"

namespace Cjing3D
{
	GameTest::GameTest(const std::shared_ptr<Engine>& engine) :
		GameComponent(engine)
	{
	}

	GameTest::~GameTest()
	{
	}

	void GameTest::Initialize()
	{
		GameComponent::Initialize();

		auto renderPath = CJING_MAKE_SHARED<RenderPathTiledForward>();
		renderPath->Initialize();
		SetRenderPath(renderPath);
	}
}