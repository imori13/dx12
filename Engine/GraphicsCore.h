#pragma once
#include "Display.h"
#include "RTVHeap.h"

class RTVHeap;

namespace Graphics
{
	extern ComPtr<ID3D12Device> g_pDevice;

	extern ComPtr<ID3D12CommandQueue> g_pCmdQueue;
	extern ComPtr<ID3D12GraphicsCommandList> g_pCmdList;

	extern ComPtr<ID3D12Resource> g_pColorBuffer[FRAME_COUNT];
	extern RTVHeap g_RtvHeap;

	bool Initialize();
	void Terminate();
	void ClearCommand();
	uint64_t ExecuteCommandList(ComPtr<ID3D12GraphicsCommandList> cmdList);
	void WaitForFence(uint64_t fenceValue);
	void IdleGpu();
}