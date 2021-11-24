#pragma once

namespace Graphics
{
	extern ComPtr<ID3D12Device> g_pDevice;

	extern ComPtr<ID3D12CommandQueue> g_pCmdQueue;
	extern ComPtr<ID3D12GraphicsCommandList> g_pCmdList;

	bool Initialize();
	void Terminate();
	void ClearCommand();
	uint64_t ExecuteCommandList(ComPtr<ID3D12GraphicsCommandList> cmdList);
	void WaitForFence(uint64_t fenceValue);
	void IdleGpu();
}