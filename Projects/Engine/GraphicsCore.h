#pragma once

namespace Graphics
{
	extern Microsoft::WRL::ComPtr<ID3D12Device> g_pDevice;

	void Initialize();
	void Terminate() noexcept;
}