#include "App_ImGui.h"
#include "WinApp.h"
#include "GraphicsCore.h"
#include "Display.h"
#include "GameCore.h"

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx12.h>

namespace App_ImGui
{
	ResourceHeap g_ResourceHeap;

	void Initialize()
	{
		g_ResourceHeap.Create(1, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		const ImGuiIO& io = ImGui::GetIO(); static_cast<void>(io);
		ImGui::StyleColorsDark();
		ImGui_ImplWin32_Init(Window::g_hWnd);
		ImGui_ImplDX12_Init(Graphics::g_pDevice.Get(), FRAME_COUNT,
							DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, g_ResourceHeap.Get(),
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
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("WindowSize : { %d ,%d }", Window::g_Width, Window::g_Height);
		ImGui::Text("AppSize : { %d ,%d }", Display::g_AppWidth, Display::g_AppHeight);
		ImGui::Text("DrawSum %f : { Update & Draw: %f , FrameWait: %f, GpuWait: %f}", GameCore::g_DrawSumT, GameCore::g_WriteT, GameCore::g_FrameWaitT, GameCore::g_GpuWaitT);
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