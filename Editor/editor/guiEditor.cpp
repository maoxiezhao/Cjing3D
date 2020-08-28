#include "guiEditor.h"
#include "core\globalContext.hpp"
#include "platform\platform.h"
#include "renderer\renderer.h"
#include "renderer\RHI\d3d11\deviceD3D11.h"
#include "editor\imgui\imguiRHI.h"
#include "helper\profiler.h"

#include "editor\widget\guiEditorWidget.h"
#include "editor\widget\guiEditorMenuBar.h"
#include "editor\widget\guiEditorToolbar.h"
#include "editor\widget\guiEditorEntityList.h"
#include "editor\widget\guiEditorProperties.h"

// TODO: Move to Imgui_RHI
#ifdef _WIN32
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

namespace Cjing3D {

	namespace {
		EditorWidget* mWidgetMenu = nullptr;
		EditorWidget* mWidgetToolbar = nullptr;
	}

	EditorStage::EditorStage()
	{
		mConfigFlag =
			ImGuiConfigFlags_NavEnableKeyboard |
			ImGuiConfigFlags_DockingEnable |
			ImGuiConfigFlags_ViewportsEnable;
	}

	EditorStage::~EditorStage()
	{
	}

	void EditorStage::Initialize()
	{
		GraphicsDevice& graphicsDevice = Renderer::GetDevice();
		if (graphicsDevice.GetGraphicsDeviceType() != GraphicsDeviceType::GraphicsDeviceType_directx11) {
			return;
		}

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= mConfigFlag;

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			io.ConfigWindowsResizeFromEdges = true;
			io.ConfigViewportsNoTaskBarIcon = true;
		}

		// Setup Platform/Renderer bindings
		GraphicsDeviceD3D11& deviceD3D11 = dynamic_cast<GraphicsDeviceD3D11&>(graphicsDevice);
		HWND hwnd = deviceD3D11.GetHwnd();
		ID3D11Device& device = deviceD3D11.GetDevice();
		ID3D11DeviceContext& deviceContext = deviceD3D11.GetDeviceContext(GraphicsThread_IMMEDIATE);

		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX11_Init(&device, &deviceContext);

		//auto window = GlobalGetEngine()->GetGameWindow();
		//if (window != nullptr) {
		//	mMessageHandler = std::make_shared<IMGUIMessageHandler>();
		//	window->AddMessageHandler(mMessageHandler);
		//}

		InitializeImpl();

		mIsInitialized = true;
	}

	void EditorStage::Uninitialize()
	{
		if (mIsInitialized == false) {
			return;
		}
		mRegisteredWindows.clear();

		for (auto& widget : mRegisteredWidgets) {
			widget->Uninitialize();
		}
		mRegisteredWidgets.clear();

		//GameWindow* window = GlobalGetEngine()->GetWindow();
		//if (window != nullptr) {
		//	mMessageHandler = std::make_shared<IMGUIMessageHandler>();
		//	window->RemoveHandler(mMessageHandler);
		//}

		// Cleanup
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		mIsInitialized = true;
	}

	void EditorStage::InitializeImpl()
	{
		RegisterCustomWidget(CJING_MAKE_SHARED<EditorWidgetMenu>(*this));
		RegisterCustomWidget(CJING_MAKE_SHARED<EditorWidgetToolbar>(*this));
		RegisterCustomWidget(CJING_MAKE_SHARED<EditorWidgetEntityList>(*this));
		RegisterCustomWidget(CJING_MAKE_SHARED<EditorWidgetProperties>(*this));

		mWidgetMenu = GetCustomWidget<EditorWidgetMenu>();
		mWidgetToolbar = GetCustomWidget<EditorWidgetToolbar>();
	}

	//////////////////////////////////////////////////////////////////////////////

	void EditorStage::FixedUpdate()
	{
		if (mIsInitialized == false) {
			return;
		}

		PROFILER_BEGIN_CPU_BLOCK("ImGuiFixedUpdate");
		if (!mIsNeedRender)
		{
			mIsNeedRender = true;

			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			FixedUpdateImpl(GetGlobalContext().GetDelatTime());
		}
		PROFILER_END_BLOCK();
	}

	void EditorStage::Render()
	{
		if (mIsInitialized == false) {
			return;
		}

		GraphicsDevice& device = Renderer::GetDevice();
		device.BeginEvent("RenderIMGUI");

		if (mIsNeedRender)
		{
			ImGui::Render();
			ImGui::EndFrame();
		}

		auto drawData = ImGui::GetDrawData();
		if (drawData != nullptr) {
			ImGui_ImplDX11_RenderDrawData(drawData);
		}

		device.EndEvent();
	}

	void EditorStage::EndFrame()
	{
		if (mIsInitialized == false) {
			return;
		}

		if (mIsNeedRender)
		{
			mIsNeedRender = false;

			if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable)
			{
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
			}
		}
	}

	void EditorStage::RegisterCustomWindow(CustomWindowFunc func)
	{
		mRegisteredWindows.push_back(func);
	}

	void EditorStage::RegisterCustomWidget(std::shared_ptr<EditorWidget> widget)
	{
		if (widget == nullptr) {
			return;
		}

		mRegisteredWidgets.push_back(widget);
		widget->Initialize();
	}


	void EditorStage::SetDockingEnable(bool isDockingEnable)
	{
		if (mIsDockingEnable != isDockingEnable)
		{
			mIsDockingEnable = isDockingEnable;
			if (mIsDockingEnable)
			{
				ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
			}
			else
			{
				ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_DockingEnable;
			}
		}
	}

	void EditorStage::FixedUpdateImpl(F32 deltaTime)
	{
		if (!IsVisible()) {
			return;
		}

		// docking begin
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable) {
			DockingBegin();
		}

		// show custom registerd windows
		for (auto it : mRegisteredWindows) {
			it(deltaTime);
		}

		// show custom widgets
		for (auto& widget : mRegisteredWidgets)
		{
			if (widget != nullptr && widget->Begin())
			{
				widget->Update(deltaTime);
				widget->End();
			}
		}

		// show imgui demo
		if (mIsShowDemo) {
			ImGui::ShowDemoWindow(&mIsShowDemo);
		}

		// docking end
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable) {
			DockingEnd();
		}
	}

	/////////////////////////////////////////////////////////////////////////////

	void EditorStage::DockingBegin()
	{
		// full screen without window
		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_MenuBar |
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::SetNextWindowBgAlpha(0.0f);

		F32 dockingOffsetY = 0.0f;
		dockingOffsetY += mWidgetToolbar->GetHeight();
		dockingOffsetY += mWidgetMenu->GetHeight();

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x,   viewport->Pos.y  + dockingOffsetY));
		ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y - dockingOffsetY));
		ImGui::SetNextWindowViewport(viewport->ID);

		static const char* dockingName = "CjingDock";
		mIsDockingBegin = ImGui::Begin(dockingName, nullptr, windowFlags);
		ImGui::PopStyleVar(3);

		if (mIsDockingBegin)
		{
			ImGuiID mainWindow = ImGui::GetID(dockingName);
			if (!ImGui::DockBuilderGetNode(mainWindow))
			{
				// reset
				ImGui::DockBuilderRemoveNode(mainWindow);
				ImGui::DockBuilderAddNode(mainWindow, ImGuiDockNodeFlags_None);
				ImGui::DockBuilderSetNodeSize(mainWindow, viewport->Size);

				ImGuiID dockMainID = mainWindow;
				// split node
				ImGuiID dockLeft  = ImGui::DockBuilderSplitNode(mainWindow, ImGuiDir_Left, 0.2f, nullptr, &dockMainID);
				ImGuiID dockRight = ImGui::DockBuilderSplitNode(mainWindow, ImGuiDir_Left, 0.6f, nullptr, &dockMainID);

				// build window
				ImGui::DockBuilderDockWindow("EntityList", dockLeft);
				ImGui::DockBuilderDockWindow("Properties", dockRight);
				ImGui::DockBuilderFinish(mainWindow);
			}

			ImGui::DockSpace(mainWindow, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
		}
	}

	void EditorStage::DockingEnd()
	{
		if (mIsDockingBegin) {
			ImGui::End();
		}
	}
}

