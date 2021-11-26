#include "Command.h"
#include "GraphicsCore.h"

void Command::Create(uint32_t swapCount)
{
	HRESULT hr;

	// コマンドキューの生成
	{
		// コマンドキューの設定
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask = 0;

		// コマンドキューの生成
		hr = Graphics::g_pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(m_pCmdQueue.GetAddressOf()));
		if(FAILED(hr))
		{ return; }
	}

	// コマンドアロケータの生成
	{
		m_pCmdAllocators = std::vector<ComPtr<ID3D12CommandAllocator>>(swapCount);
		m_pCmdAllocators.reserve(swapCount);

		for(auto i = 0u; i < swapCount; ++i)
		{

			hr = Graphics::g_pDevice->CreateCommandAllocator(
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				IID_PPV_ARGS(m_pCmdAllocators[i].GetAddressOf()));	// ダブルバッファリング用で2個
			if(FAILED(hr))
			{ return; }
		}
	}

	// コマンドリストの生成
	{
		hr = Graphics::g_pDevice->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			m_pCmdAllocators[0].Get(),
			nullptr,	// パイプラインステート(後で設定する)
			IID_PPV_ARGS(m_pCmdList.GetAddressOf()));
		if(FAILED(hr))
		{ return; }
	}

	// フェンスの生成
	{
		// フェンスの生成
		hr = Graphics::g_pDevice->CreateFence(
			static_cast<UINT64>(0),
			D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(m_pFence.GetAddressOf()));
		if(FAILED(hr))
		{ return; }

		// フェンスイベントの生成
		m_FenceEventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if(m_FenceEventHandle == nullptr)
		{ return; }
	}

	m_pCmdList->Close();
}

const ComPtr<ID3D12GraphicsCommandList>& Command::Begin(uint32_t SwapIndex)
{
	ClearCommand(SwapIndex);

	return m_pCmdList;
}

void Command::Finish()
{
	auto fenceValue = ExecuteCommandList();
	WaitForFence(fenceValue);
}

// private method ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void Command::ClearCommand(uint32_t SwapIndex)
{
	// コマンドの記録を開始
	m_pCmdAllocators[SwapIndex]->Reset();
	m_pCmdList->Reset(m_pCmdAllocators[SwapIndex].Get(), nullptr);
}

// TODO : マルチバッファに対応する(書き込み直後のバッファは待たず、書き込み予定のバッファだけ待てばいい)
uint64_t Command::SignalQueue()
{
	m_pCmdQueue->Signal(m_pFence.Get(), m_NextFenceValue);
	return m_NextFenceValue++;
}

uint64_t Command::ExecuteCommandList()
{
	m_pCmdList->Close();

	ID3D12CommandList* ppCmdLists[] = { m_pCmdList.Get() };
	m_pCmdQueue->ExecuteCommandLists(1, ppCmdLists);
	
	return SignalQueue();
}

void Command::WaitForGpu()
{
	auto fenceValue = SignalQueue();

	WaitForFence(fenceValue);
}

void Command::WaitForFence(uint64_t fenceValue)
{
	if(m_pFence->GetCompletedValue() >= fenceValue)
	{ return; }

	m_pFence->SetEventOnCompletion(fenceValue, m_FenceEventHandle);
	WaitForSingleObjectEx(m_FenceEventHandle, INFINITE, FALSE);
}

