#include "imguiStage.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"

#include "core\globalContext.hpp"
#include "platform\gameWindow.h"
#include "renderer\renderer.h"
#include "renderer\RHI\d3d11\deviceD3D11.h"
#include "gui\guiEditor\guiEditorWidget.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
namespace Cjing3D
{
	namespace {
		class IMGUIMessageHandler : public WindowMessageHandler
		{
		public:
			IMGUIMessageHandler() {}
			virtual~IMGUIMessageHandler() {}

			virtual bool HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT& result)
			{
				if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
					return true;

				return false;
			}
		};
		std::shared_ptr<IMGUIMessageHandler> mMessageHandler = nullptr;
	}

	IMGUIStage::IMGUIStage(GUIRenderer& renderer):
		mIsInitialized(false),
		mRenderer(renderer)
	{
	}

	IMGUIStage::~IMGUIStage()
	{
	}

	void IMGUIStage::Initialize()
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

		GameWindow* window = GlobalGetEngine()->GetWindow();
		if (window != nullptr) {
			mMessageHandler = std::make_shared<IMGUIMessageHandler>();
			window->AddMessageHandler(mMessageHandler);
		}

		mRenderer.SetImGuiStage(this);
		InitializeImpl();

		mIsInitialized = true;
	}
	void IMGUIStage::Uninitialize()
	{
		if (mIsInitialized == false) {
			return;
		}
		mRegisteredWindows.clear();

		for (auto& widget : mRegisteredWidgets) {
			widget->Uninitialize();
		}
		mRegisteredWidgets.clear();

		GameWindow* window = GlobalGetEngine()->GetWindow();
		if (window != nullptr) {
			mMessageHandler = std::make_shared<IMGUIMessageHandler>();
			window->RemoveHandler(mMessageHandler);
		}

		// Cleanup
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		mIsInitialized = true;
	}
	void IMGUIStage::Update(F32 deltaTime)
	{
		if (mIsInitialized == false) {
			return;
		}
	}

	void IMGUIStage::FixedUpdate()
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

			UpdateImpl(GlobalGetDeltaTime());
		}
		PROFILER_END_BLOCK();
	}

	void IMGUIStage::Render()
	{
		if (mIsInitialized == false) {
			return;
		}
	}

	void IMGUIStage::EndFrame()
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

	void IMGUIStage::RenderImpl()
	{
		GraphicsDevice& device = Renderer::GetDevice();
		device.BeginEvent("RenderIMGUI");
		
		if (mIsNeedRender) 
		{
			mIsNeedRender = false;
			ImGui::Render();
			ImGui::EndFrame();
		}

		auto drawData = ImGui::GetDrawData();
		if (drawData != nullptr) {
			ImGui_ImplDX11_RenderDrawData(drawData);
		}

		device.EndEvent();
	}

	

	void IMGUIStage::RegisterCustomWindow(CustomWindowFunc func)
	{
		mRegisteredWindows.push_back(func);
	}

	void IMGUIStage::RegisterCustomWidget(std::shared_ptr<EditorWidget> widget)
	{
		if (widget == nullptr) {
			return;
		}

		mRegisteredWidgets.push_back(widget);
		widget->Initialize();
	}

	void IMGUIStage::DockingBegin()
	{
	}

	void IMGUIStage::DockingEnd()
	{
	}

	void IMGUIStage::InitializeImpl()
	{
	}

	void IMGUIStage::SetDockingEnable(bool isDockingEnable)
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

	void IMGUIStage::UpdateImpl(F32 deltaTime)
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
		if (mIsShowDemo)  {
			ImGui::ShowDemoWindow(&mIsShowDemo);
		}

		// docking end
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable) {
			DockingEnd();
		}
	}
}