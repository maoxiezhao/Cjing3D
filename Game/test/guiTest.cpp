#include "guiTest.h"

#include "utils\signal\signal.h"

#include "gui\guiStage.h"
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
#include "gui\guiWidgets\image.h"
#include "gui\guiWidgets\ninePatchImage.h"
#include "gui\guiWidgets\textEdit.h"
#include "gui\guiWidgets\inputField.h"

namespace Cjing3D
{
void GUITest::DoTest()
{
	auto& guiStage = GlobalGetSubSystem<GUIStage>();
	//guiStage.SetIsDebugDraw(true);

	Gui::WidgetHierarchy& widgetHierarchy = guiStage.GetWidgetHierarchy();
	// window
	//auto window = std::make_shared<Gui::Window>(guiStage, StringID("checkBoxlabel1"), u8"GUI Test", F32x2{200.0f, 300.0f});	
	//window->SetPos({ Renderer::GetScreenSize()[0] - 200.0f - 5.0f, 200.0f});
	//widgetHierarchy.AddWidget(window);

	auto baseWindow = std::make_shared<Gui::Window>(guiStage, StringID("checkBoxlabel1"), u8"Base Widget", F32x2{ 300.0f, 400.0f });
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
		auto popupButton = std::make_shared<Gui::PopupButton>(guiStage, StringID("Popup"), "Popup");
		baseWindow->Add(popupButton);

		auto popup = popupButton->GetPopup();
		popup->SetLayout(std::make_shared<Gui::BoxLayout>(
			Gui::AlignmentOrien_Vertical,
			Gui::AlignmentMode_Center,
			Gui::WidgetMargin(5.0f),
			5.0f
			));
		{
			label = std::make_shared<Gui::Label>(guiStage, StringID("scrollViewLabel"), "ScrollView");
			popup->Add(label);
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
			auto scrollView = std::make_shared<Gui::ScrollView>(guiStage, StringID("ScrollView"), F32x2(100.0f, 150.0f));
			scrollView->SetPos({ 400.0f, 100.0f });
			scrollView->SetWidget(stackPanel);
			popup->Add(scrollView);
		}

		// input text
		label = std::make_shared<Gui::Label>(guiStage, StringID("TextFieldLabel"), "TextField");
		baseWindow->Add(label);
		auto textEdit = std::make_shared<Gui::TextField>(guiStage, StringID("TextField"));
		textEdit->SetWidth(80.0f);
		baseWindow->Add(textEdit);

		// image
		label = std::make_shared<Gui::Label>(guiStage, StringID("imageLabel"), "Normal Image");
		baseWindow->Add(label);

		auto imagePanel = std::make_shared<Gui::Widget>(guiStage, StringID("imagePanel"));
		imagePanel->SetLayout(std::make_shared<Gui::BoxLayout>(
			Gui::AlignmentOrien_Horizontal,
			Gui::AlignmentMode_Center,
			Gui::WidgetMargin(5.0f),
			5.0f));
		for (int i = 0; i < 2; i++)
		{
			auto image = std::make_shared<Gui::Image>(guiStage, StringID("image" + std::to_string(i)));
			image->SetTexture("Textures/logo.png");
			image->SetSizeAndFixedSize({ 60.0f, 60.0f });
			image->SetExpand(true);
			image->SetStretchMode(Gui::Image::STRETCH_KEEP_ASPECT);
			imagePanel->Add(image);
		}
		baseWindow->Add(imagePanel);

		// nine patch image
		label = std::make_shared<Gui::Label>(guiStage, StringID("9imageLabel"), "Nine Patch Image");
		baseWindow->Add(label);

		auto nineImage = std::make_shared<Gui::NinePatchImage>(guiStage, StringID("nineimage"));
		nineImage->SetTexture("Textures/logo.png");
		nineImage->SetSize({ 160.0f, 60.0f });
		nineImage->SetPatchRect({ 40.0f, 20.0f, 40.0f, 20.0f });
		baseWindow->Add(nineImage);
	}
	widgetHierarchy.AddWidget(baseWindow);


	widgetHierarchy.UpdateLayout();
}
}