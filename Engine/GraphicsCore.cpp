#include "GraphicsCore.h"
#include "Display.h"

namespace
{
	ComPtr<ID3D12CommandAllocator> s_pCmdAllocator[FRAME_COUNT] = { nullptr };
	ComPtr<ID3D12Fence> s_pFence = nullptr;
	uint64_t s_NextFenceValue = 0;
	HANDLE s_FenceEventHandle = nullptr;
}

namespace Graphics
{
	ComPtr<ID3D12Device> g_pDevice = nullptr;
	ComPtr<ID3D12CommandQueue> g_pCmdQueue = nullptr;
	ComPtr<ID3D12GraphicsCommandList> g_pCmdList = nullptr;

	bool Initialize()
	{
		HRESULT hr;

		// デバッグレイヤーの有効化
#if defined(DEBUG) || defined(_DEBUG)
		{
			ComPtr<ID3D12Debug> debug;
			hr = D3D12GetDebugInterface(IID_PPV_ARGS(debug.GetAddressOf()));

			// デバッグレイヤーを有効化
			if(SUCCEEDED(hr))
			{ debug->EnableDebugLayer(); }
		}
#endif

		// デバイスの生成
		hr = D3D12CreateDevice(
			nullptr,
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(g_pDevice.GetAddressOf()));
		if(FAILED(hr))
		{ return false; }

		// コマンドキューの生成
		{
			// コマンドキューの設定
			D3D12_COMMAND_QUEUE_DESC desc = {};
			desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
			desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			desc.NodeMask = 0;

			// コマンドキューの生成
			hr = Graphics::g_pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(g_pCmdQueue.GetAddressOf()));
			if(FAILED(hr))
			{ return false; }
		}

		// コマンドアロケータの生成
		{
			for(auto i = 0u; i < FRAME_COUNT; ++i)
			{
				hr = g_pDevice->CreateCommandAllocator(
					D3D12_COMMAND_LIST_TYPE_DIRECT,
					IID_PPV_ARGS(s_pCmdAllocator[i].GetAddressOf()));	// ダブルバッファリング用で2個
				if(FAILED(hr))
				{ return false; }
			}
		}

		// コマンドリストの生成
		{
			hr = g_pDevice->CreateCommandList(
				0,
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				s_pCmdAllocator[0].Get(),
				nullptr,	// パイプラインステート(後で設定する)
				IID_PPV_ARGS(g_pCmdList.GetAddressOf()));
			if(FAILED(hr))
			{ return false; }
		}

		// フェンスの生成
		{
			// フェンスの生成
			hr = Graphics::g_pDevice->CreateFence(
				static_cast<UINT64>(0),
				D3D12_FENCE_FLAG_NONE,
				IID_PPV_ARGS(s_pFence.GetAddressOf()));
			if(FAILED(hr))
			{ return false; }

			// フェンスイベントの生成
			s_FenceEventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
			if(s_FenceEventHandle == nullptr)
			{ return false; }
		}

		g_pCmdList->Close();

		return true;
	}

	void ClearCommand()
	{
		// コマンドの記録を開始
		auto index = Display::s_FrameIndex;
		s_pCmdAllocator[index]->Reset();
		g_pCmdList->Reset(s_pCmdAllocator[index].Get(), nullptr);
	}

	void Terminate()
	{
		// フェンス破棄
		s_pFence.Reset();

		// コマンドリストの破棄
		g_pCmdList.Reset();

		// コマンドアロケータの破棄
		for(auto i = 0u; i < FRAME_COUNT; ++i)
		{ s_pCmdAllocator[i].Reset(); }

		// コマンドキューの破棄
		g_pCmdQueue.Reset();

		// デバイスの破棄
		g_pDevice.Reset();
	}

	uint64_t ExecuteCommandList(ComPtr<ID3D12GraphicsCommandList> cmdList)
	{
		cmdList->Close();

		ID3D12CommandList* ppCmdLists[] = { cmdList.Get() };
		g_pCmdQueue->ExecuteCommandLists(1, ppCmdLists);
		g_pCmdQueue->Signal(s_pFence.Get(), s_NextFenceValue);

		return s_NextFenceValue++;
	}

	void WaitForFence(uint64_t fenceValue)
	{
		if(s_pFence->GetCompletedValue() >= fenceValue)
		{ return; }

		s_pFence->SetEventOnCompletion(fenceValue, s_FenceEventHandle);
		WaitForSingleObjectEx(s_FenceEventHandle, INFINITE, FALSE);
	}

	uint64_t IncrementFence()
	{
		g_pCmdQueue->Signal(s_pFence.Get(), s_NextFenceValue);
		return s_NextFenceValue++;
	}

	void IdleGpu()
	{
		WaitForFence(IncrementFence());
	}
}