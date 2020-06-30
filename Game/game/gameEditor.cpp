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
#include "gui\guiWidgets\layouts\gridLayout.h"
#include "gui\guiWidgets\popup.h"
#include "gui\guiWidgets\scrollView.h"

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
		Font::LoadFontTTF("Fonts/arial.ttf");

		auto& guiStage = GlobalGetSubSystem<GUIStage>();
		//guiStage.SetIsDebugDraw(true);

#ifdef _ENABLE_GAME_EDITOR_
		Editor::InitializeEditor(guiStage.GetImGUIStage());
#endif // _ENABLE_GAME_EDITOR_

		Gui::WidgetHierarchy& widgetHierarchy = guiStage.GetWidgetHierarchy();
		// window
		auto window = std::make_shared<Gui::Window>(guiStage, StringID("checkBoxlabel1"), u8"GUI Test", F32x2{200.0f, 300.0f});	
		window->SetPos({ Renderer::GetScreenSize()[0] - 200.0f - 5.0f, 200.0f});
		widgetHierarchy.AddWidget(window);

		auto baseWindow = std::make_shared<Gui::Window>(guiStage, StringID("checkBoxlabel1"), u8"Base Widget", F32x2{ 300.0f, 300.0f });
		baseWindow->SetPos({ (Renderer::GetScreenSize()[0] - 300.0f - 5.0f) * 0.5f, 200.0f });
		auto layout = std::make_shared<Gui::SimpleGridLayout>(
			Gui::AlignmentOrien_Horizontal,
			Gui::AlignmentMode_Center,
			2,
			Gui::WidgetMargin(5.0f, 5.0f, 5.0f, 5.0f),
			10.0f
		);
		layout->SetColAlignment({ Gui::AlignmentMode_End, Gui::AlignmentMode_Begin });
		layout->SetDefaultRowAlignment(Gui::AlignmentMode_Center);
		baseWindow->SetLayout(layout);
		{
			// label
			auto labelLabel = std::make_shared<Gui::Label>(guiStage, StringID("label11"), "Label");
			baseWindow->Add(labelLabel);
			auto labelLabe2 = std::make_shared<Gui::Label>(guiStage, StringID("label22"), "Hello World!");
			baseWindow->Add(labelLabe2);

			// button
			auto label = std::make_shared<Gui::Label>(guiStage, StringID("label"), "Button");
			baseWindow->Add(label);
			std::shared_ptr<Gui::Button> button1 = std::make_shared<Gui::Button>(guiStage, StringID("Push"), "Push");
			baseWindow->Add(button1);		

			// check box
			label = std::make_shared<Gui::Label>(guiStage, StringID("label1"), "CheckBox");
			baseWindow->Add(label);
			auto widget = std::make_shared<Gui::CheckBox>(guiStage, StringID("checkBox"));
			baseWindow->Add(widget);

			// popup box
			label = std::make_shared<Gui::Label>(guiStage, StringID("label2"), "Popup");
			baseWindow->Add(label);
			auto popupButton = std::make_shared<Gui::PopupButton>(guiStage, StringID("Popup") , "Popup");
			baseWindow->Add(popupButton);

			auto popup = popupButton->GetPopup();
			popup->SetLayout(std::make_shared<Gui::BoxLayout>(
				Gui::AlignmentOrien_Vertical,
				Gui::AlignmentMode_Center,
				Gui::WidgetMargin(5.0f),
				5.0f
			));
			for (int i = 0; i < 3; i++) {
				auto button = std::make_shared<Gui::Button>(guiStage, StringID("Push" + std::to_string(i)), "Push" + std::to_string(i));
				popup->Add(button);
			}
		}
		//widgetHierarchy.AddWidget(baseWindow);

		// list
		auto stackPanel = std::make_shared<Gui::StackPanel>(guiStage, StringID("StackPanel"));
		stackPanel->SetMargin(Gui::WidgetMargin(5.0f));
		stackPanel->SetSpacing(5.0f);
		for (int i = 0; i < 10; i++)
		{
			auto button = std::make_shared<Gui::Button>(guiStage, StringID("Button" + std::to_string(i)), "Button" + std::to_string(i));
			stackPanel->Add(button);
		}
	
		// scroll view
		auto scrollView = std::make_shared<Gui::ScrollView>(guiStage, StringID("ScrollView"), F32x2(100.0f, 100.0f));
		scrollView->SetPos({ 400.0f, 50.0f });
		scrollView->SetWidget(stackPanel);
		widgetHierarchy.AddWidget(scrollView);

		widgetHierarchy.UpdateLayout();
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