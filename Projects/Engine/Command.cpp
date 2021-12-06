#include "Command.h"
#include "Display.h"

void Command::Create(uint32_t swapCount)
{
	EXPECTS(0 <= swapCount && swapCount < 10);

	HRESULT hr{};

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
		ENSURES(hr, "CommandQueue生成");
	}

	// コマンドアロケータの生成
	{
		m_pCmdAllocators = std::vector<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>>(FRAME_COUNT);
		m_pCmdAllocators.reserve(FRAME_COUNT);
		for(auto& itr : m_pCmdAllocators)
		{
			hr = Graphics::g_pDevice->CreateCommandAllocator(
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				IID_PPV_ARGS(itr.GetAddressOf()));	// ダブルバッファリング用で2個
			ENSURES(hr, "CommandAllocator生成");
		}
	}

	// コマンドリストの生成
	{
		hr = Graphics::g_pDevice->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			m_pCmdAllocators.at(0).Get(),
			nullptr,	// パイプラインステート(後で設定する)
			IID_PPV_ARGS(m_pCmdList.GetAddressOf()));
		ENSURES(hr, "CommandList生成");
	}

	// フェンスの生成
	{
		// フェンスの生成
		hr = Graphics::g_pDevice->CreateFence(
			static_cast<uint64_t>(0),
			D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(m_pFence.GetAddressOf()));
		ENSURES(hr, "Fence生成");

		// フェンスイベントの生成
		m_FenceEventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		ENSURES(m_FenceEventHandle != nullptr, "FenceEvent生成");
	}

	m_pCmdList->Close();
}

const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& Command::Begin(uint32_t SwapIndex)
{
	ClearCommand(SwapIndex);

	return m_pCmdList;
}

void Command::Finish()
{
	const auto fenceValue = ExecuteCommandList();
	WaitForFence(fenceValue);
}

// private method ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void Command::ClearCommand(uint32_t SwapIndex)
{
	// コマンドの記録を開始
	m_pCmdAllocators.at(SwapIndex)->Reset();
	m_pCmdList->Reset(m_pCmdAllocators.at(SwapIndex).Get(), nullptr);
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
	const auto span = gsl::make_span(ppCmdLists);

	m_pCmdQueue->ExecuteCommandLists(1, span.data());

	return SignalQueue();
}

void Command::WaitForGpu()
{
	const auto fenceValue = SignalQueue();

	WaitForFence(fenceValue);
}

void Command::WaitForFence(uint64_t fenceValue)
{
	if(m_pFence->GetCompletedValue() >= fenceValue)
	{ return; }

	m_pFence->SetEventOnCompletion(fenceValue, m_FenceEventHandle);
	WaitForSingleObjectEx(m_FenceEventHandle, INFINITE, FALSE);
}

