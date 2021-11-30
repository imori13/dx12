#pragma once
#include "Command.h"

class Command;

namespace Graphics
{
	extern Microsoft::WRL::ComPtr<ID3D12Device> g_pDevice;
	extern Command g_Command;

	void Initialize();
	void Terminate() noexcept;
}