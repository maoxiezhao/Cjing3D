#include "imguiStage.h"
#include "imgui\imgui.h"
#include "imgui\imgui_impl_win32.h"
#include "imgui\imgui_impl_dx11.h"

#include "engine.h"
#include "core\systemContext.hpp"
#include "renderer\renderer.h"
#include "renderer\renderer2D.h"
#include "renderer\RHI\deviceD3D11.h"
#include "utils\baseWindow.h"

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

	/***************************************************************************************/

	namespace {
		void ShowTransformWindow(TransformComponent& transform)
		{
			ImGui::Text("Transform");

			XMFLOAT3 translation = transform.GetTranslationLocal();
			float vec3f[3] = { translation.x, translation.y, translation.z };
			if (ImGui::DragFloat3("position", vec3f, 0.005f, -1000, 1000))
			{
				translation.x = vec3f[0];
				translation.y = vec3f[1];
				translation.z = vec3f[2];

				transform.SetTranslationLocal(translation);
			}

			XMFLOAT4 quaternion = transform.GetRotationLocal();
			XMFLOAT3 rotation = QuaternionToRollPitchYaw(quaternion);
			float rVec3f[3] = { rotation.x, rotation.y, rotation.z };
			if (ImGui::DragFloat3("rotation", rVec3f, 0.01f, -XM_2PI, XM_2PI))
			{
				rotation.x = rVec3f[0];
				rotation.y = rVec3f[1];
				rotation.z = rVec3f[2];

				transform.SetRotateFromRollPitchYaw(rotation);
			}

			XMFLOAT3 scale = transform.GetScaleLocal();
			float sVec3f[3] = { scale.x, scale.y, scale.z };
			if (ImGui::DragFloat3("scale", sVec3f, 0.005f, 0, 10))
			{
				scale.x = sVec3f[0];
				scale.y = sVec3f[1];
				scale.z = sVec3f[2];

				transform.SetScaleLocal(scale);
			}
		}

		Entity currentObject = INVALID_ENTITY;
		void ShowObjectAttribute()
		{
			if (currentObject == INVALID_ENTITY) return;

			ImGui::SetNextWindowPos(ImVec2(1070, 20), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(350, 360), ImGuiCond_Always);
			ImGui::Begin("Object attributes");

			Scene& scene = Scene::GetScene();
			// transform
			auto transform = scene.mTransforms.GetComponent(currentObject);
			if (transform != nullptr) {
				ShowTransformWindow(*transform);
			}

			ImGui::End();
		}
	
		Entity currentLight = INVALID_ENTITY;
		void ShowLightAttribute()
		{
			if (currentLight == INVALID_ENTITY) return;

			ImGui::SetNextWindowPos(ImVec2(1070, 20), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(350, 360), ImGuiCond_Always);
			ImGui::Begin("Light attributes");

			Scene& scene = Scene::GetScene();
			// transform
			auto transform = scene.mTransforms.GetComponent(currentLight);
			if (transform != nullptr) {
				ShowTransformWindow(*transform);
			}

			// light 
			auto light = scene.mLights.GetComponent(currentLight);
			if (light != nullptr)
			{
				ImGui::Spacing();
				ImGui::Text("Light");

				F32 color[3] = {light->mColor[0], light->mColor[1], light->mColor[2] };
				if (ImGui::ColorEdit3("color", color))
				{
					light->mColor[0] = color[0];
					light->mColor[1] = color[1];
					light->mColor[2] = color[2];
				}

				F32 energy = light->mEnergy;
				if (ImGui::DragFloat("energy", &energy, 0.1f, 9.0f, 200.0f))
				{
					light->mEnergy = energy;
				}
			}

			ImGui::End();
		}


		void ShowFrameAttribute()
		{
		}

		StringID mCurrentAttributeWindow = STRING_ID(object);
		std::vector < std::function<void()>> mRegisteredWindowFuncs;
	}

	void IMGUIStage::InitializeImpl()
	{
		mRegisteredWindowFuncs.push_back(ShowObjectAttribute);
		mRegisteredWindowFuncs.push_back(ShowLightAttribute);
	}

	void IMGUIStage::UpdateImpl(F32 deltaTime)
	{
		// show base window
		if (IsShowBasicInfo())
		{
			ImGui::SetNextWindowPos(ImVec2(10, 20), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(350, 150), ImGuiCond_Always);
			ImGui::Begin("Application info");
			ImGui::Text("Cjing3D v0.0.1");
			ImGui::Text("");
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", deltaTime, 1.0f / deltaTime);
			ImGui::Text("");
			ImGui::Text("F4-Show Debug; F5-save scene; F6-load scene");
			ImGui::Text("WASD-Move camear  RB-Rotate camear");
			ImGui::End();
		}

		if (!IsShowDetailInfo()) {
			return;
		}

		//bool openDemoWindow = true;
		//ImGui::ShowDemoWindow(&openDemoWindow);
		{
			ImGui::SetNextWindowPos(ImVec2(10, 180), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(350, 360), ImGuiCond_Always);
			ImGui::Begin("Entity Window");

			Scene& scene = Scene::GetScene();
			ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
			if (ImGui::BeginTabBar("EntityBar", tab_bar_flags))
			{
				static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

				currentObject = INVALID_ENTITY;
				currentLight = INVALID_ENTITY;

				// show object window
				if (ImGui::BeginTabItem("Objects"))
				{
					static int objectSelectionIndex = -1;
					int nodeClicked = -1;
					auto objectManager = scene.GetComponentManager<ObjectComponent>();
					for (int index = 0; index < objectManager.GetCount(); index++)
					{
						Entity entity = objectManager.GetEntityByIndex(index);

						std::string nodeName = "Entity ";
						auto nameComponent = scene.mNames.GetComponent(entity);
						if (nameComponent != nullptr) {
							nodeName = nodeName + " " + nameComponent->GetString();
						}
						else {
							nodeName = nodeName + std::to_string(entity);
						}

						// 通过mask检测是否点击到
						ImGuiTreeNodeFlags node_flags = base_flags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
						const bool is_selected = (objectSelectionIndex  == index);
						if (is_selected) {
							node_flags |= ImGuiTreeNodeFlags_Selected;
							currentObject = entity;
						}
						
						ImGui::TreeNodeEx((void*)(intptr_t)index, node_flags, nodeName.c_str());
						if (ImGui::IsItemClicked()) {
							nodeClicked = index;
						}
					}

					if (nodeClicked != -1) {
						objectSelectionIndex = nodeClicked;
					}

					ImGui::EndTabItem();
				}
				// show light window
				if (ImGui::BeginTabItem("Lights"))
				{
					static int lightSelectionIndex = -1;
					int nodeClicked = -1;
					auto lightManager = scene.GetComponentManager<LightComponent>();
					for (int index = 0; index < lightManager.GetCount(); index++)
					{
						Entity entity = lightManager.GetEntityByIndex(index);

						std::string nodeName = "Entity ";
						auto nameComponent = scene.mNames.GetComponent(entity);
						if (nameComponent != nullptr) {
							nodeName = nodeName + " " + nameComponent->GetString();
						}
						else {
							nodeName = nodeName + std::to_string(entity);
						}

						ImGuiTreeNodeFlags node_flags = base_flags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
						const bool is_selected = (lightSelectionIndex == index);
						if (is_selected) {
							node_flags |= ImGuiTreeNodeFlags_Selected;
							currentLight = entity;
						}

						ImGui::TreeNodeEx((void*)(intptr_t)index, node_flags, nodeName.c_str());
						if (ImGui::IsItemClicked()) {
							nodeClicked = index;
						}
					}

					if (nodeClicked != -1) {
						lightSelectionIndex = nodeClicked;
					}

					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}

			ImGui::End();
		}

		// show attribute window
		{
			for (auto it : mRegisteredWindowFuncs) {
				it();
			}
		}
	}
}