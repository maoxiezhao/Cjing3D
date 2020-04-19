#include "imguiStage.h"
#include "imgui\imgui.h"
#include "imgui\imgui_impl_win32.h"
#include "imgui\imgui_impl_dx11.h"

#include "gui\guiEditor\guiEditor.h"

#include "engine.h"
#include "utils\baseWindow.h"
#include "core\systemContext.hpp"
#include "renderer\renderer.h"
#include "renderer\renderer2D.h"
#include "renderer\RHI\deviceD3D11.h"
#include "renderer\paths\renderPath3D.h"

#include "system\sceneSystem.h"

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

	IMGUIStage::IMGUIStage():
		mIsInitialized(false)
	{
	}

	IMGUIStage::~IMGUIStage()
	{
	}

	void IMGUIStage::Initialize()
	{
		SystemContext& systemContext = SystemContext::GetSystemContext();
		Renderer& renderer = systemContext.GetSubSystem<Renderer>();
		
		GraphicsDevice& graphicsDevice = renderer.GetDevice();
		if (graphicsDevice.GetGraphicsDeviceType() != GraphicsDeviceType::GraphicsDeviceType_directx11) {
			return;
		}

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		// Setup Platform/Renderer bindings
		GraphicsDeviceD3D11& deviceD3D11 = dynamic_cast<GraphicsDeviceD3D11&>(graphicsDevice);
		HWND hwnd = deviceD3D11.GetHwnd();
		ID3D11Device& device = deviceD3D11.GetDevice();
		ID3D11DeviceContext& deviceContext = deviceD3D11.GetDeviceContext(GraphicsThread_IMMEDIATE);
	
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX11_Init(&device, &deviceContext);

		BaseWindow* window = systemContext.GetEngine()->GetWindow();
		if (window != nullptr) {
			mMessageHandler = std::make_shared<IMGUIMessageHandler>();
			window->AddMessageHandler(mMessageHandler);
		}

		InitializeImpl();

		mIsInitialized = true;
	}
	void IMGUIStage::Uninitialize()
	{
		if (mIsInitialized == false) {
			return;
		}

		mRegisteredWindows.clear();

		SystemContext& systemContext = SystemContext::GetSystemContext();
		BaseWindow* window = systemContext.GetEngine()->GetWindow();
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

		SystemContext& systemContext = SystemContext::GetSystemContext();
		Renderer& renderer = systemContext.GetSubSystem<Renderer>();

		Renderer2D& renderer2D = renderer.GetRenderer2D();
		RenderPath2D* path = renderer2D.GetCurrentRenderPath();
		if (path == nullptr) {
			return;
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		UpdateImpl(deltaTime);
	}

	void IMGUIStage::Render(GUIRenderer& renderer)
	{
		if (mIsInitialized == false) {
			return;
		}

		renderer.SetImGuiStage(this);
	}

	void IMGUIStage::RenderImpl()
	{
		SystemContext& systemContext = SystemContext::GetSystemContext();
		Renderer& renderer = systemContext.GetSubSystem<Renderer>();
		GraphicsDevice& device = renderer.GetDevice();

		device.BeginEvent("RenderIMGUI");
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		device.EndEvent();
	}

	void IMGUIStage::RegisterCustomWindow(CustomWindowFunc func)
	{
		mRegisteredWindows.push_back(func);
	}

	void IMGUIStage::InitializeImpl()
	{
		Editor::InitializeEditor(*this);
	}

	void IMGUIStage::UpdateImpl(F32 deltaTime)
	{
		if (!IsShowDetailInfo()) {
			return;
		}

		// show base window
		if (IsShowBasicInfo())
		{
			Editor::ShowBasicWindow(deltaTime);
		}

		// show custom registerd windows
		for (auto it : mRegisteredWindows) {
			it();
		}
	}
}