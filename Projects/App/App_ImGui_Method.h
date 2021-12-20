#pragma once
#include "DataAverage.h"
#include <imgui.h>
#include <implot.h>

namespace AppGui
{
	void DebugViewEnable()
	{
		const double update = DataAverage::Get(L"Update");
		const double render = DataAverage::Get(L"Render");
		const double present = DataAverage::Get(L"Present");
		const double gpuWait = DataAverage::Get(L"GPUwait");

		ImGui::SetNextWindowSize(ImVec2(350, 560), ImGuiCond_Always);
		ImGui::Begin("DebugWindow");
		{
			ImGui::Text("Hello World!");
			ImGui::Text("WindowSize : W %d  H %d", WinApp::g_Width, WinApp::g_Height);
			ImGui::Text("AppSize    : W %d  H %d", Display::g_AppWidth, Display::g_AppHeight);

			ImGui::Text("Second : %.1lf", Timer::g_ElapsedTime);
			ImGui::Text("FPS    : %.1f (deltaT:%.2fms)", 1.f / DataAverage::Get(L"FPS"), gsl::narrow_cast<float>(DataAverage::Get(L"FPS") * 1000.f));

			ImGui::Text("Draw   : %.2fms %.2fms", DataAverage::Get(L"çXêVéûä‘"), update + render + present + gpuWait);
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
		const double update = DataAverage::Get(L"Update");
		const double render = DataAverage::Get(L"Render");
		const double present = DataAverage::Get(L"Present");
		const double gpuWait = DataAverage::Get(L"GPUwait");

		static std::vector<double> updateTimes;
		static std::vector<double> renderTimes;
		static std::vector<double> presentTimes;
		static std::vector<double> waitGpuTimes;
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
			ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.1f);
			ImPlot::PlotShaded("Update_", updateTimes.data(), FillModeLimit);
			ImPlot::PlotShaded("Render_", renderTimes.data(), FillModeLimit);
			ImPlot::PlotShaded("Present_", presentTimes.data(), FillModeLimit);
			ImPlot::PlotShaded("Gpuwait_", waitGpuTimes.data(), FillModeLimit);
			ImPlot::PopStyleVar();
			ImPlot::PlotLine("Update_", updateTimes.data(), FillModeLimit);
			ImPlot::PlotLine("Render_", renderTimes.data(), FillModeLimit);
			ImPlot::PlotLine("Present_", presentTimes.data(), FillModeLimit);
			ImPlot::PlotLine("Gpuwait_", waitGpuTimes.data(), FillModeLimit);
			ImPlot::EndPlot();
		}
	}

	void FramePieEnable()
	{
		const double update = DataAverage::Get(L"Update");
		const double render = DataAverage::Get(L"Render");
		const double present = DataAverage::Get(L"Present");
		const double gpuWait = DataAverage::Get(L"GPUwait");

		static std::array< const char*, 4> label = { "Update" ,"Render" ,"Present", "GPUwait" };
		static std::vector<double> datas = { 0,0,0,0 };
		datas.at(0) = update;
		datas.at(1) = render;
		datas.at(2) = present;
		datas.at(3) = gpuWait;

		ImPlot::SetNextPlotLimits(0, 1, 0, 1, ImGuiCond_Always);
		if(ImPlot::BeginPlot("##Pie2"))
		{
			ImPlot::PlotPieChart(label.data(), datas.data(), gsl::narrow<int>(datas.size()), 0.5, 0.5, 0.4, false, "%.1f");
			ImPlot::EndPlot();
		}
	}

	void FrameViewrEnable()
	{
		constexpr uint32_t size = 1000u;
		static std::vector<float> fps;
		fps.insert(fps.begin(), DataAverage::Get(L"FPS"));
		if(!fps.empty() && fps.size() > size) { fps.pop_back(); }

		ImPlot::SetNextPlotLimits(0, 1000, 0, 0.02f, ImGuiCond_Always);
		if(ImPlot::BeginPlot("LineGraph"))
		{
			ImPlot::PlotLine("FPS", fps.data(), gsl::narrow<int>(fps.size()));
			ImPlot::EndPlot();
		}
	}
}