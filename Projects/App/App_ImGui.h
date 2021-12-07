#pragma once
#include "ResourceHeap.h"

namespace App_ImGui
{
	extern ResourceHeap g_ResourceHeap;

	void Initialize();
	void Update();
	void Render(gsl::not_null<ID3D12GraphicsCommandList*> cmdList);
	void Terminate();
};

