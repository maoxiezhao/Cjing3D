#include "guiEditor.h"
#include "editor\imgui\imguiRHI.h"
#include "core\eventSystem.h"
#include "platform\win32\gameWindowWin32.h"
#include "renderer\RHI\d3d11\deviceD3D11.h"
#include "helper\profiler.h"

#include "editor\widget\guiEditorWidget.h"
#include "editor\widget\guiEditorMenuBar.h"
#include "editor\widget\guiEditorToolbar.h"
#include "editor\widget\guiEditorEntityList.h"
#include "editor\widget\guiEditorProperties.h"
#include "editor\widget\guiEditorViewport.h"

// TODO: Move to Imgui_RHI
#ifdef _WIN32
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

namespace Cjing3D {

	namespace {
		EditorWidget* mWidgetMenu = nullptr;
		EditorWidget* mWidgetToolbar = nullptr;
	}

	EditorStage::EditorStage(GameWindow& gameWindow) :
		mGameWindow(gameWindow)
	{
	}

	EditorStage::~EditorStage()
	{
	}

	void EditorStage::Initialize()
	{
		if (mIsInitialized) {
			return;
		}

		GraphicsDevice& graphicsDevice = Renderer::GetDevice();
		if (graphicsDevice.GetGraphicsDeviceType() != GraphicsDeviceType::GraphicsDeviceType_directx11) {
			return;
		}

		auto gameWindowWin32 = dynamic_cast<Win32::GameWindowWin32*>(&mGameWindow);
		if (gameWindowWin32 == nullptr) {
			return;
		}

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags = 
			ImGuiConfigFlags_NavEnableKeyboard |
			ImGuiConfigFlags_DockingEnable |
			ImGuiConfigFlags_ViewportsEnable;

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			io.ConfigWindowsResizeFromEdges = true;
			io.ConfigViewportsNoTaskBarIcon = true;
		}

		HWND hwnd = gameWindowWin32->GetHwnd();
		ImGui_ImplWin32_Init(hwnd);
		ImGui::RHI::Initialize();

		gameWindowWin32->AddMessageHandler([this](const Win32::WindowMessageData& window) {
			ImGui_ImplWin32_WndProcHandler(window.handle, window.message, window.wparam, window.lparam);
			return false;
		});

		InitializeWidgets();

		if (!mResolutionChangedConn.IsConnected())
		{
			auto screenSize = Renderer::GetScreenSize();
			CreateViewportRenderTarget(screenSize[0], screenSize[1]);
			mResolutionChangedConn = EventSystem::Register(EVENT_RESOLUTION_CHANGE,
				[this](const VariantArray& variants) {
					const U32 width = variants[0].GetValue<U32>();
					const U32 height = variants[1].GetValue<U32>();

					CreateViewportRenderTarget(width, height);
				});
		}

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

		ImGui::RHI::Uninitilize();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		mIsInitialized = true;
	}

	void EditorStage::InitializeWidgets()
	{
		RegisterCustomWidget(CJING_MAKE_SHARED<EditorWidgetMenu>(*this));
		RegisterCustomWidget(CJING_MAKE_SHARED<EditorWidgetToolbar>(*this));
		RegisterCustomWidget(CJING_MAKE_SHARED<EditorWidgetEntityList>(*this));
		RegisterCustomWidget(CJING_MAKE_SHARED<EditorWidgetProperties>(*this));
		RegisterCustomWidget(CJING_MAKE_SHARED<EditorWidgetViewport>(*this));

		mWidgetMenu = GetCustomWidget<EditorWidgetMenu>();
		mWidgetToolbar = GetCustomWidget<EditorWidgetToolbar>();
	}

	//////////////////////////////////////////////////////////////////////////////

	void EditorStage::Render(CommandList cmd)
	{
		if (mIsInitialized == false) {
			return;
		}

		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		FixedUpdateImpl(GetGlobalContext().GetDelatTime());

		Renderer::GetDevice().BeginEvent(cmd, "RenderIMGUI");
		ImGui::Render();
		ImGui::RHI::Render(cmd, ImGui::GetDrawData());
		Renderer::GetDevice().EndEvent(cmd);
	}

	void EditorStage::EndFrame()
	{
		if (mIsInitialized == false) {
			return;
		}

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
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

	void EditorStage::CreateViewportRenderTarget(U32 width, U32 height)
	{
		auto& device = Renderer::GetDevice();

		TextureDesc desc = {};
		desc.mWidth = width;
		desc.mHeight = height;
		desc.mFormat = device.GetBackBufferFormat();
		desc.mSampleDesc.mCount = 1;
		desc.mBindFlags = BIND_RENDER_TARGET | BIND_SHADER_RESOURCE;
		{
			const auto result = device.CreateTexture2D(&desc, nullptr, mViewportRenderTarget);
			Debug::ThrowIfFailed(result, "Failed to create viewport render target:%08x", result);
			device.SetResourceName(mViewportRenderTarget, "ViewportRT");
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
				ImGuiID dockLeft  = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Left,  0.2f, nullptr, &dockMainID);
				ImGuiID dockRight = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Right, 0.2f, nullptr, &dockMainID);

				// build window
				ImGui::DockBuilderDockWindow("EntityList", dockLeft);
				ImGui::DockBuilderDockWindow("Properties", dockRight);
				ImGui::DockBuilderDockWindow("Viewport",   dockMainID);
				ImGui::DockBuilderFinish(dockMainID);
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

