#include "App_ImGui.h"
#include "WinApp.h"
#include "GraphicsCore.h"
#include "Display.h"
#include "GameCore.h"
#include "Timer.h"
#include "DataAverage.h"

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx12.h>

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

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

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
		ImGui_ImplDX12_Init(Graphics::g_pDevice.Get(), FRAME_COUNT,
							DXGI_FORMAT_R8G8B8A8_UNORM, g_ResourceHeap.Get(),
							g_ResourceHeap.GetCPUHandle(0),
							g_ResourceHeap.GetGPUHandle(0));

	}

	void Update()
	{
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void Render(gsl::not_null<ID3D12GraphicsCommandList*> cmdList)
	{
		ImGui::Begin("Test");
		//ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar
		ImGui::Text("Hello World!");
		ImGui::Text("WindowSize : W %d  H %d", Window::g_Width, Window::g_Height);
		ImGui::Text("AppSize    : W %d  H %d", Display::g_AppWidth, Display::g_AppHeight);

		ImGui::Text("Second : %.1lf", Timer::g_ElapsedTime);
		ImGui::Text("FPS    : %.1f (deltaT:%.2fms)", 1.f / DataAverage::Get(L"FPS"), gsl::narrow_cast<float>(DataAverage::Get(L"FPS") * 1000.f));

		const double update = DataAverage::Get(L"Update");
		const double render = DataAverage::Get(L"Render");
		const double present = DataAverage::Get(L"Present");
		const double syncHz = DataAverage::Get(L"SyncHz");
		const double gpuWait = DataAverage::Get(L"GPUwait");
		ImGui::Text("Draw   : %.2fms %.2fms", DataAverage::Get(L"XVŽžŠÔ"), update + render + present + syncHz + gpuWait);
		ImGui::Text("  Update  : %.2fms", update);
		ImGui::Text("  Render  : %.2fms", render);
		ImGui::Text("  Present : %.2fms", present);
		ImGui::Text("  SyncHz  : %.2fms", syncHz);
		ImGui::Text("  GPUwait : %.2fms", gpuWait);
		ImGui::Checkbox("Sync", &GameCore::g_IsSync);

		ImGui::End();

		ImGui::Begin("Hoge");
		ImGui::Text("Docking Test");
		ImGui::End();

		ImGui::Render();

		cmdList->SetDescriptorHeaps(1, g_ResourceHeap.GetHeapAddress());
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList);
	}

	void Terminate()
	{
		ImGui_ImplWin32_Shutdown();
		ImGui_ImplDX12_Shutdown();
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