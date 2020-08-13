#include "imguiStage.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "engine.h"
#include "platform\gameWindow.h"
#include "core\systemContext.hpp"
#include "renderer\renderer.h"
#include "renderer\2D\renderer2D.h"
#include "renderer\RHI\d3d11\deviceD3D11.h"
#include "renderer\paths\renderPath3D.h"

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
		GraphicsDevice& graphicsDevice = Renderer::GetDevice();
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

		GameWindow* window = GlobalGetEngine()->GetWindow();
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

		if (!mIsNeedRender)
		{
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			mIsNeedRender = true;

			UpdateImpl(GlobalGetDeltaTime());
		}
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

	void IMGUIStage::InitializeImpl()
	{
	}

	void IMGUIStage::UpdateImpl(F32 deltaTime)
	{
		if (!IsVisible()) {
			return;
		}

		// show custom registerd windows
		for (auto it : mRegisteredWindows) {
			it(deltaTime);
		}

		//bool openDemoWindow = true;
		//ImGui::ShowDemoWindow(&openDemoWindow);
	}
}