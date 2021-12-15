#include "App_ImGui.h"
#include "WinApp.h"
#include "GraphicsCore.h"
#include "Display.h"
#include "GameCore.h"
#include "Timer.h"
#include "DataAverage.h"
#include "Command.h"

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx12.h>
#include <implot.h>

namespace
{
	bool s_ViewportsEnable;
}

namespace App_ImGui
{
	ResourceHeap g_ResourceHeap;

	void Initialize()
	{
		g_ResourceHeap.Create(1, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

		ImGui::CreateContext();
		ImPlot::CreateContext();
		ImPlot::GetStyle().AntiAliasedLines = true;

		auto& io = ImGui::GetIO();
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

		ImGui::StyleColorsDark();
		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();

		s_ViewportsEnable = (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable);

		if(s_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		ImGui_ImplWin32_Init(Window::g_hWnd);

		const auto& handle = g_ResourceHeap.GetNextHandle();
		ImGui_ImplDX12_Init(Graphics::g_pDevice.Get(), FRAME_COUNT,
							DXGI_FORMAT_R8G8B8A8_UNORM, g_ResourceHeap.Get(),
							handle.CPU, handle.GPU);
	}

	void Update()
	{
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void Render(gsl::not_null<ID3D12GraphicsCommandList*> cmdList)
	{
		static double update{};
		static double render{};
		static double present{};
		static double gpuWait{};
		update = DataAverage::Get(L"Update");
		render = DataAverage::Get(L"Render");
		present = DataAverage::Get(L"Present");
		gpuWait = DataAverage::Get(L"GPUwait");

		ImGui::Begin("DebugWindow");
		{
			ImGui::Text("Hello World!");
			ImGui::Text("WindowSize : W %d  H %d", Window::g_Width, Window::g_Height);
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

			{
				static std::vector<float> fps;
				fps.emplace_back(DataAverage::Get(L"FPS"));
				constexpr uint32_t size = 1000;
				if(!fps.empty() && fps.size() > size) { fps.erase(fps.begin()); }

				ImPlot::SetNextPlotLimits(0, 1000, 0, 0.02f, ImGuiCond_Always);
				if(ImPlot::BeginPlot("LineGraph"), NULL, NULL, ImVec2(50, 50), 0, ImPlotAxisFlags_Time)
				{
					ImPlot::PlotLine("FPS", fps.data(), gsl::narrow<int>(fps.size()));
					ImPlot::EndPlot();
				}
			}
		}
		ImGui::End();

		static std::array< const char*, 4> label = { "Update" ,"Render" ,"Present", "GPUwait" };
		static std::vector<double> datas = { 0,0,0,0 };
		datas.at(0) = update;
		datas.at(1) = render;
		datas.at(2) = present;
		datas.at(3) = gpuWait;

		ImGui::Begin("AAAAA");
		{
			ImGui::Text("%.1lf ms", (datas.at(0) + datas.at(1) + datas.at(2) + datas.at(3)));

			ImPlot::SetNextPlotLimits(0, 1, 0, 1, ImGuiCond_Always);
			if(ImPlot::BeginPlot("##Pie2"))
			{
				ImPlot::PlotPieChart(label.data(), datas.data(), gsl::narrow<int>(datas.size()), 0.5, 0.5, 0.4, false, "%.1f");
				ImPlot::EndPlot();
			}

			static std::vector<double> updateTimes;
			static std::vector<double> renderTimes;
			static std::vector<double> presentTimes;
			static std::vector<double> waitGpuTimes;
			const uint32_t FillModeLimit = std::min(1000u, static_cast<uint32_t>(waitGpuTimes.size()) + 1);
			updateTimes.emplace_back(update);
			renderTimes.emplace_back(render + update);
			presentTimes.emplace_back(present + render + update);
			waitGpuTimes.emplace_back(gpuWait + present + render + update);
			if(!updateTimes.empty() && updateTimes.size() > FillModeLimit) { updateTimes.erase(updateTimes.begin()); }
			if(!renderTimes.empty() && renderTimes.size() > FillModeLimit) { renderTimes.erase(renderTimes.begin()); }
			if(!presentTimes.empty() && presentTimes.size() > FillModeLimit) { presentTimes.erase(presentTimes.begin()); }
			if(!waitGpuTimes.empty() && waitGpuTimes.size() > FillModeLimit) { waitGpuTimes.erase(waitGpuTimes.begin()); }

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
		ImGui::End();

		ImGui::Render();

		cmdList->SetDescriptorHeaps(1, g_ResourceHeap.GetAddress());
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList);
	}

	void Terminate()
	{
		ImGui_ImplWin32_Shutdown();
		ImGui_ImplDX12_Shutdown();
		ImPlot::DestroyContext();
		ImGui::DestroyContext();
	}

	void UpdateAdditionalPlatformWindows(gsl::not_null<ID3D12GraphicsCommandList*> cmdList)
	{
		if(s_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault(nullptr, static_cast<void*>(cmdList));
		}
	}
}

// imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
	{ return true; }

	switch(message)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}

		case WM_SIZE:
		{
			if(Graphics::g_pDevice != nullptr)
			{
				RECT windowRect = {};
				GetWindowRect(hWnd, &windowRect);
				Window::g_Width = windowRect.right - windowRect.left;
				Window::g_Height = windowRect.bottom - windowRect.top;

				RECT clientRect = {};
				GetClientRect(hWnd, &clientRect);
				Display::OnSizeChanged(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);
			}
			break;
		}

		case WM_SYSKEYDOWN:
		{
			if((wParam == VK_RETURN) && (lParam & (1 << 29)) && Graphics::g_pDevice != nullptr)
			{
				Display::ToggleFullscreen();
			}
			break;
		}

		default:
		{
			break;
		}
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}