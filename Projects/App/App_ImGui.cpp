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

		ImGuiIO& io = ImGui::GetIO(); static_cast<void>(io);
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigDockingWithShift = true;

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
		ImGui::Begin("DebugWindow");
		//ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar
		ImGui::Text("Hello World!");
		ImGui::Text("WindowSize : W %d  H %d", Window::g_Width, Window::g_Height);
		ImGui::Text("AppSize    : W %d  H %d", Display::g_AppWidth, Display::g_AppHeight);

		ImGui::Text("Second : %.1lf", Timer::g_ElapsedTime);
		ImGui::Text("FPS    : %.1f (deltaT:%.2fms)", 1.f / DataAverage::Get(L"FPS"), gsl::narrow_cast<float>(DataAverage::Get(L"FPS") * 1000.f));

		static double update{};
		static double render{};
		static double present{};
		static double gpuWait{};
		update = DataAverage::Get(L"Update");
		render = DataAverage::Get(L"Render");
		present = DataAverage::Get(L"Present");
		gpuWait = DataAverage::Get(L"GPUwait");
		ImGui::Text("Draw   : %.2fms %.2fms", DataAverage::Get(L"�X�V����"), update + render + present + gpuWait);
		{
			ImGui::Indent();
			ImGui::Text("Update  : %.2fms", update);
			ImGui::Text("Render  : %.2fms", render);
			ImGui::Text("Present : %.2fms", present);
			ImGui::Text("GPUwait : %.2fms", gpuWait);
			ImGui::Unindent();
		}
		ImGui::Checkbox("Sync", &GameCore::g_IsSync);

		ImGui::End();

		static std::vector<float> fps{};
		fps.emplace_back(DataAverage::Get(L"FPS"));
		constexpr uint32_t size = 1000;
		if(!fps.empty() && fps.size() > size)
		{
			fps.erase(fps.begin());
		}

		static bool normalize = false;
		static std::array< const char*, 4> label = { "Update" ,"Render" ,"Present", "GPUwait" };
		static std::vector<double> datas = { 0,0,0,0 };
		datas.at(0) = update;
		datas.at(1) = render;
		datas.at(2) = present;
		datas.at(3) = gpuWait;

		{
			ImPlot::SetNextAxisToFit(ImAxis_X1);
		}

		ImGui::Begin("AAAAA");
		ImGui::Text("%.1lf ms", (datas.at(0) + datas.at(1) + datas.at(2)));
		if(ImPlot::BeginPlot("##Pie1", ImVec2(250, 250), ImPlotFlags_Equal | ImPlotFlags_NoMouseText))
		{
			ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_NoDecorations, ImPlotAxisFlags_NoDecorations);
			ImPlot::SetupAxesLimits(0, 1, 0, 1);
			ImPlot::PlotPieChart(label.data(), datas.data(), gsl::narrow<int>(datas.size() - 1), 0.5, 0.5, 0.4, normalize, "%.1f");
			ImPlot::EndPlot();
		}

		ImGui::Text("%.1lf ms", (datas.at(0) + datas.at(1) + datas.at(2) + datas.at(3)));
		if(ImPlot::BeginPlot("##Pie2", ImVec2(250, 250), ImPlotFlags_Equal | ImPlotFlags_NoMouseText))
		{
			ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_NoDecorations, ImPlotAxisFlags_NoDecorations);
			ImPlot::SetupAxesLimits(0, 1, 0, 1);
			ImPlot::PlotPieChart(label.data(), datas.data(), gsl::narrow<int>(datas.size()), 0.5, 0.5, 0.4, normalize, "%.1f");
			ImPlot::EndPlot();
		}
		ImGui::End();


		ImGui::Begin("LineGraphWindow");
		if(ImPlot::BeginPlot("LineGraph"))
		{
			ImPlot::SetupAxesLimits(0, size, 0, 0.02f);
			ImPlot::PlotLine("FPS", fps.data(), gsl::narrow<int>(fps.size()));
			ImPlot::EndPlot();
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