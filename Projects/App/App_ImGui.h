#pragma once
#include "ResourceHeap.h"
#include "Vector3.h"
#include "Command.h"

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx12.h>
#include <implot.h>

namespace App_ImGui
{
	extern ResourceHeap g_ResourceHeap;

	void Initialize();
	void Update();
	void Render(gsl::not_null<ID3D12GraphicsCommandList*> cmdList);
	void Terminate();
	void UpdateAdditionalPlatformWindows(gsl::not_null<ID3D12GraphicsCommandList*> cmdList);

	void Begin(std::string_view name);
	void CreateSlider(std::string_view name, gsl::not_null<float*> value);
	void CreateSlider(std::string_view name, gsl::not_null<Vector3*> vec);
	void DragSlider(std::string_view name, gsl::not_null<float*> value);
	void DragSlider(std::string_view name, gsl::not_null<Vector3*> vec);
	void End();
};

