#include "gameEditor.h"

#include "engine.h"
#include "scripts\luaContext.h"
#include "system\sceneSystem.h"
#include "renderer\paths\renderPath_tiledForward.h"
#include "gui\guiStage.h"
#include "audio\audio.h"
#include "renderer\renderer.h"
#include "renderer\2D\renderer2D.h"
#include "renderer\2D\font.h"

#include "editor\guiEditor.h"
#include "editor\guiEditorScene.h"

#include <thread>
#include <Windows.h>

#include "utils\string\utf8String.h"
#include "renderer\2D\textDrawable.h"

// test
#include "utils\signal\signal.h"
#include "gui\guiWidgets\stackPanel.h"
#include "gui\guiWidgets\button.h"
#include "gui\guiWidgets\label.h"
#include "gui\guiWidgets\listPanel.h"
#include "gui\guiWidgets\checkBox.h"
#include "gui\guiWidgets\layouts\boxLayout.h"
#include "gui\guiWidgets\window.h"

namespace Cjing3D
{
	GameEditor::GameEditor()
	{
	}

	GameEditor::~GameEditor()
	{
	}

	TextDrawable textDrawable;

	void GameEditor::Initialize()
	{
		RenderPathTiledForward* path = new RenderPathTiledForward();
		Renderer::SetCurrentRenderPath(path);
		Font::LoadFontTTF("Fonts/NotoSans-Regular.ttf");
		Font::LoadFontTTF("Fonts/NotoSans-Bold.ttf");

		auto& guiStage = GlobalGetSubSystem<GUIStage>();
		//guiStage.SetIsDebugDraw(true);

#ifdef _ENABLE_GAME_EDITOR_
		Editor::InitializeEditor(guiStage.GetImGUIStage());
#endif // _ENABLE_GAME_EDITOR_

		Gui::WidgetHierarchy& widgetHierarchy = guiStage.GetWidgetHierarchy();
		// Test GUI
		//Gui::WidgetPtr stack = std::make_shared<Gui::StackPanel>(guiStage, StringID("test"), 200.0f, 300.0f);
		//stack->SetPos({ 500.0f, 400.0f });
		//widgetHierarchy.AddWidget(stack);

		//// button
		//std::shared_ptr<Gui::Button> button1 = std::make_shared<Gui::Button>(guiStage, StringID("button"));
		//button1->SetText("Button");
		//stack->Add(button1);

		//// list panel
		//auto listPanel = std::make_shared<Gui::ListPanel>(guiStage, "ListPane", 160.0f, 200.0f);
		//listPanel->SetLayoutSpacing(2);

		//for (int i = 0; i < 6; i++)
		//{
		//	const std::string name = "label" + std::to_string(i);
		//	auto label = std::make_shared<Gui::Label>(guiStage, StringID(name), "ListItem" + std::to_string(i));
		//	listPanel->Add(label);
		//}
		//stack->Add(listPanel);

		//// label
		//auto label = std::make_shared<Gui::Label>(guiStage, StringID("label"), "MotherFuck");
		//label->SetTextAlignH(Font::TextAlignH_Center);
		//stack->Add(label);


		// check box
		//auto widget = std::make_shared<Gui::Widget>(guiStage, "checkBoxPanel");
		//widget->SetLayout(std::make_shared<Gui::BoxLayout>(Gui::AlignmentOrien::AlignmentOrien_Horizontal, Gui::AlignmentMode::AlignmentMode_Center, Gui::WidgetMargin(), 10.0f));
		//
		//auto label = std::make_shared<Gui::Label>(guiStage, StringID("checkBoxlabel"), "checkBox");
		//widget->Add(label);

		// window
		auto window = std::make_shared<Gui::Window>(guiStage, StringID("checkBoxlabel1"), u8"GUI Test Window", F32x2{200.0f, 300.0f});	
		window->SetPos({ Renderer::GetScreenSize()[0] - 200.0f - 5.0f, 200.0f});
		window->GetTitleTextDrawable().SetFontStyle("Fonts/NotoSans-Bold.ttf");
		widgetHierarchy.AddWidget(window);
	}

	void GameEditor::Update(EngineTime time)
	{
	}

	void GameEditor::FixedUpdate()
	{

#ifdef _ENABLE_GAME_EDITOR_
		auto& inputManager = GlobalGetSubSystem<InputManager>();
		if (inputManager.IsKeyDown(KeyCode::Esc)) {
			GlobalGetEngine()->SetIsExiting(true);
		}

		if (inputManager.IsKeyDown(KeyCode::F4))
		{
			auto& guiStage = GlobalGetSubSystem<GUIStage>();
			bool show = guiStage.IsImGUIStageVisible();
			guiStage.SetImGUIStageVisible(!show);
		}

		// test scene serializing
		if (inputManager.IsKeyDown(KeyCode::F5)) 
		{
			//Editor::LoadSceneFromOpenFile();
		}
		else if (inputManager.IsKeyDown(KeyCode::F6)) 
		{
			Editor::SaveSceneToOpenFile();
		}
		else if (inputManager.IsKeyDown(KeyCode::F7))
		{
			Scene::GetScene().Clear();
		}
#endif // _ENABLE_GAME_EDITOR_

	}

	void GameEditor::Uninitialize()
	{
	}
}