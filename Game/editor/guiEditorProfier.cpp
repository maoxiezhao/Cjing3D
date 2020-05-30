#include "guiEditorProfier.h"

#include "gui\guiEditor\guiEditorInclude.h"

#include <queue>

namespace Cjing3D
{
namespace Editor
{
	U32 maxDeltaTimeCount = 20;
	std::vector<F32> mUpdateDeltaTimes;
	std::vector<F32> mRenderDeltaTimes;

	bool bShowCPUProfilerWindow = false;
	void ShowCPUProfilerWindowImpl(F32 deltaTime)
	{
		if (bShowCPUProfilerWindow == false) return;

		ImGui::SetNextWindowPos(ImVec2(500, 100), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(350, 400), ImGuiCond_Once);
		ImGui::Begin("CPU Profiler", &bShowCPUProfilerWindow);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", deltaTime, 1.0f / deltaTime);
		ImGui::Separator();
		ImGui::Text("Memory:");
		ImGui::Text("heap memory usage:%.3f KB", Memory::GetMemUsage() / 1000.0f);
		ImGui::Text("----------------------------");
		ImGui::Text("");
		ImGui::Text(Profiler::GetInstance().GetCPUProfileString().c_str());
		ImGui::Text("");

		auto& profiler = Profiler::GetInstance();
		char deltaTimeTitle[32];
		
		// update deltaTime
		F32 updateDeltaTime = profiler.GetCPUBlockDeltaTime(STRING_ID(Update)) / 1000.0f;
		mUpdateDeltaTimes.push_back(updateDeltaTime);
		if (mUpdateDeltaTimes.size() >= maxDeltaTimeCount) {
			mUpdateDeltaTimes.erase(mUpdateDeltaTimes.begin());
		}
		sprintf(deltaTimeTitle, "time %f", updateDeltaTime);
		ImGui::PlotHistogram("Update Times", mUpdateDeltaTimes.data(), mUpdateDeltaTimes.size(), 0, deltaTimeTitle, 0.0f, 0.005f, ImVec2(0, 60));

		// render delatTime
		F32 renderDeltaTime = profiler.GetCPUBlockDeltaTime(STRING_ID(Render)) / 1000.0f;
		mRenderDeltaTimes.push_back(renderDeltaTime);
		if (mRenderDeltaTimes.size() >= maxDeltaTimeCount) {
			mRenderDeltaTimes.erase(mRenderDeltaTimes.begin());
		}
		sprintf(deltaTimeTitle, "time %f", renderDeltaTime);
		ImGui::PlotHistogram("Render Times", mRenderDeltaTimes.data(), mRenderDeltaTimes.size(), 0, deltaTimeTitle, 0.0f, 0.005f, ImVec2(0, 60));


		ImGui::End();
	}


	bool bShowGPUProfilerWindow = false;
	void ShowGPUProfilerWindowImpl(F32 deltaTime)
	{
		if (bShowGPUProfilerWindow == false) return;

		ImGui::SetNextWindowPos(ImVec2(500, 100), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(350, 400), ImGuiCond_Once);
		ImGui::Begin("GPU Profiler", &bShowGPUProfilerWindow);

		ImGui::Text(Profiler::GetInstance().GetGPUProfileString().c_str());
		ImGui::Text("");

		ImGui::End();
	}

	void InitializeEditorProfiler(IMGUIStage& imguiStage)
	{
		imguiStage.RegisterCustomWindow(ShowCPUProfilerWindowImpl);
		imguiStage.RegisterCustomWindow(ShowGPUProfilerWindowImpl);
	}

	void ShowCPUProfilerWindow()
	{
		bShowCPUProfilerWindow = true;
	}

	void ShowGPUProfilerWindow()
	{
		bShowGPUProfilerWindow = true;
	}
}
}


