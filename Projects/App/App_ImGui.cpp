#include "App_ImGui.h"
#include "WinApp.h"
#include "GraphicsCore.h"
#include "Display.h"
#include "GameCore.h"
#include "Timer.h"
#include "DataAverage.h"
#include "Command.h"
#include "App_ImGui_Method.h"

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx12.h>
#include <implot.h>

namespace
{
	bool s_ViewportsEnable;
	bool s_GUI_ENABLE = true;
}

namespace App_ImGui
{
	ResourceHeap g_ResourceHeap;

	void Initialize()
	{
		if(!s_GUI_ENABLE) { return; }

		g_ResourceHeap.Create(1, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

		ImGui::CreateContext();
		ImPlot::CreateContext();
		ImPlot::GetStyle().AntiAliasedLines = true;

		auto& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

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
		if(!s_GUI_ENABLE) { return; }
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		//AppGui::DebugViewEnable();
		AppGui::FillEnable();
		AppGui::FramePieEnable();
		//AppGui::FrameViewrEnable();
	}

	void Render(gsl::not_null<ID3D12GraphicsCommandList*> cmdList)
	{
		if(!s_GUI_ENABLE) { return; }

		ImGui::Render();

		cmdList->SetDescriptorHeaps(1, g_ResourceHeap.GetAddress());
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList);
	}

	void Terminate()
	{
		if(!s_GUI_ENABLE) { return; }
		ImGui_ImplWin32_Shutdown();
		ImGui_ImplDX12_Shutdown();
		ImPlot::DestroyContext();
		ImGui::DestroyContext();
	}

	void UpdateAdditionalPlatformWindows(gsl::not_null<ID3D12GraphicsCommandList*> cmdList)
	{
		if(!s_GUI_ENABLE) { return; }
		if(s_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault(nullptr, static_cast<void*>(cmdList));
		}
	}

	void Begin(std::string_view name)
	{
		ImGui::Begin(name.data());
	}

	void CreateSlider(std::string_view name, gsl::not_null<float*> value)
	{
		ImGui::SliderFloat(name.data(), value, -10, 10);
	}

	void CreateSlider(std::string_view name, gsl::not_null<Vector3*> vec)
	{
		ImGui::SliderFloat3(name.data(), vec->data(), -10, 10);
	}

	void DragSlider(std::string_view name, gsl::not_null<float*> value)
	{
		ImGui::DragFloat(name.data(), value);
	}
	void DragSlider(std::string_view name, gsl::not_null<Vector3*> vec)
	{
		ImGui::DragFloat3(name.data(), vec->data());
	}

	void End()
	{
		ImGui::End();
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