#pragma once
#include "Display.h"

class Command
{
public:
	Command() noexcept
		: m_pCmdQueue(nullptr)
		, m_pCmdList(nullptr)
		, m_pFence(nullptr)
		, m_NextFenceValue(0)
		, m_FenceEventHandle(nullptr)
	{

	}

	// 生成処理
	void Create(uint32_t swapCount);

	// 描画開始
	const ComPtr<ID3D12GraphicsCommandList>& Begin(uint32_t SwapIndex);
	// 描画終了
	void Finish();

	// GPU待機
	void WaitForGpu();

	ID3D12CommandQueue* GetCmdQueue() noexcept { return m_pCmdQueue.Get(); }
private:
	ComPtr<ID3D12CommandQueue> m_pCmdQueue;
	ComPtr<ID3D12GraphicsCommandList> m_pCmdList;
	std::vector<ComPtr<ID3D12CommandAllocator>> m_pCmdAllocators;
	ComPtr<ID3D12Fence> m_pFence;

	uint64_t m_NextFenceValue;
	HANDLE m_FenceEventHandle;

	void ClearCommand(uint32_t SwapCount);
	uint64_t SignalQueue();
	uint64_t ExecuteCommandList();
	void WaitForFence(uint64_t fenceValue);
};