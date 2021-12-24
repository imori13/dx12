#pragma once
#include "DataAverage.h"
#include <imgui.h>
#include <implot.h>

namespace
{
	float update;
	float render;
	float present;
	float gpuWait;

	std::vector<float> updateTimes;
	std::vector<float> renderTimes;
	std::vector<float> presentTimes;
	std::vector<float> waitGpuTimes;

	constexpr std::array<const char*, 4> label = { "WaitGPU","Present","Render" ,"Update" };
	std::vector<float> datas = { 0,0,0,0 };

	std::vector<float> fps;
}

namespace AppGui
{
	void Update()
	{
		update = DataAverage::Get(L"Update");
		render = DataAverage::Get(L"Render");
		present = DataAverage::Get(L"Present");
		gpuWait = DataAverage::Get(L"GPUwait");
	}

	void DebugViewEnable()
	{

		ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_Once);
		ImGui::Begin("DebugWindow");
		{
			ImGui::Text("Hello World!");
			ImGui::Text("WindowSize : W %d  H %d", WinApp::g_Width, WinApp::g_Height);
			ImGui::Text("AppSize    : W %d  H %d", Display::g_AppWidth, Display::g_AppHeight);

			ImGui::Text("Second : %.1lf", Timer::g_ElapsedTime);
			ImGui::Text("FPS    : %.1f (deltaT:%.2fms)", 1.f / DataAverage::Get(L"FPS"), gsl::narrow_cast<float>(DataAverage::Get(L"FPS") * 1000.f));

			ImGui::Text("Draw   : %.2fms %.2fms", DataAverage::Get(L"çXêVéûä‘"), 0.0 + update + render + present + gpuWait);
			{
				ImGui::Indent();
				ImGui::Text("Update  : %.2fms", update);
				ImGui::Text("Render  : %.2fms", render);
				ImGui::Text("Present : %.2fms", present);
				ImGui::Text("GPUwait : %.2fms", gpuWait);
				ImGui::Unindent();
			}
			ImGui::Checkbox("Sync", &GameCore::g_IsSync);
		}
		ImGui::End();
	}

	void FillEnable()
	{
		const uint32_t FillModeLimit = std::min(1000u, static_cast<uint32_t>(waitGpuTimes.size()) + 1);
		updateTimes.insert(updateTimes.begin(), update);
		renderTimes.insert(renderTimes.begin(), render + update);
		presentTimes.insert(presentTimes.begin(), present + render + update);
		waitGpuTimes.insert(waitGpuTimes.begin(), gpuWait + present + render + update);

		if(!updateTimes.empty() && updateTimes.size() > FillModeLimit) { updateTimes.pop_back(); }
		if(!renderTimes.empty() && renderTimes.size() > FillModeLimit) { renderTimes.pop_back(); }
		if(!presentTimes.empty() && presentTimes.size() > FillModeLimit) { presentTimes.pop_back(); }
		if(!waitGpuTimes.empty() && waitGpuTimes.size() > FillModeLimit) { waitGpuTimes.pop_back(); }

		ImPlot::SetNextPlotLimitsX(0u, 1000u, ImGuiCond_Always);
		ImPlot::SetNextPlotLimitsY(0u, 20u, ImGuiCond_Always);
		if(ImPlot::BeginPlot("FillMode"))
		{
			ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 1.0f);
			ImPlot::PlotShaded("WaitGPU", waitGpuTimes.data(), FillModeLimit);
			ImPlot::PlotShaded("Present", presentTimes.data(), FillModeLimit);
			ImPlot::PlotShaded("Render", renderTimes.data(), FillModeLimit);
			ImPlot::PlotShaded("Update", updateTimes.data(), FillModeLimit);
			ImPlot::PopStyleVar();
			ImPlot::PlotLine("WaitGPU", waitGpuTimes.data(), FillModeLimit);
			ImPlot::PlotLine("Present", presentTimes.data(), FillModeLimit);
			ImPlot::PlotLine("Render", renderTimes.data(), FillModeLimit);
			ImPlot::PlotLine("Update", updateTimes.data(), FillModeLimit);
			ImPlot::EndPlot();
		}
	}

	void FramePieEnable()
	{
		datas = { gpuWait ,present,render,update };

		ImPlot::SetNextPlotLimits(0, 1, 0, 1, ImGuiCond_Always);
		if(ImPlot::BeginPlot("Pie2"))
		{
			ImPlot::PlotPieChart(label.data(), datas.data(), gsl::narrow<int>(datas.size()), 0.5f, 0.5f, 0.4f, true);
			ImPlot::EndPlot();
		}
	}

	void FrameViewrEnable()
	{
		constexpr uint32_t size = 1000u;
		const auto data = DataAverage::Get(L"FPS");
		fps.insert(fps.begin(), data);
		if(!fps.empty() && fps.size() > size) { fps.pop_back(); }

		ImPlot::SetNextPlotLimits(0, 1000, 0, 0.02f, ImGuiCond_Always);
		if(ImPlot::BeginPlot("FrameGraph"))
		{
			ImPlot::PlotLine("Frame", fps.data(), gsl::narrow<int>(fps.size()));
			ImPlot::EndPlot();
		}
	}
}