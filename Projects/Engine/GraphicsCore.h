#pragma once
#include "Command.h"
#include "ResourceHeap.h"

class Command;

namespace Graphics
{
	extern Microsoft::WRL::ComPtr<ID3D12Device> g_pDevice;
	extern Command g_Command;
	extern ResourceHeap g_ResourceHeap;

	void Initialize();
	void Terminate() noexcept;
}