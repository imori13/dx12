#pragma once
#include "ResourceHeap.h"

namespace Graphics
{
	extern Microsoft::WRL::ComPtr<ID3D12Device> g_pDevice;
	extern ResourceHeap g_ResourceHeap;

	void Initialize();
	void Terminate() noexcept;
}