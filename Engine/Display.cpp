#include "Display.h"
#include "GraphicsCore.h"
#include <cassert>

namespace
{
	ComPtr<IDXGISwapChain3> s_pSwapChain = nullptr;
	ComPtr<ID3D12Resource> s_pColorBuffer[FRAME_COUNT] = { nullptr };
	ComPtr<ID3D12DescriptorHeap> s_pHeapRTV = nullptr;
	//D3D12_CPU_DESCRIPTOR_HANDLE s_HandleRTV[FRAME_COUNT] = { 0 };

	ComPtr<ID3D12Fence> s_pFence = nullptr;
	//HANDLE s_FenceEvent = nullptr;

	ComPtr<ID3D12CommandQueue> s_pQueue = nullptr;	// 一時的にdisplayで記述

	uint32_t s_FrameIndex = 0;
	uint64_t s_FrameCount = FRAME_COUNT;
}

#define CURRENT_BUFFER s_FrameCount
#define PREV_BUFFER s_FrameCount - FRAME_COUNT

namespace Display
{
	void WaitForGpu(ComPtr<ID3D12Fence> fence, uint64_t value)
	{

		// 次のフレームの描画準備がまだであれば待機する
		if(fence->GetCompletedValue() < value)
		{
			HANDLE fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
			if(fenceEvent != nullptr)
			{
				fence->SetEventOnCompletion(value, fenceEvent);
				WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);
			}
		}
	}

	bool Initialize(void)
	{
		// コマンドキューの生成
		{
			// コマンドキューの設定
			D3D12_COMMAND_QUEUE_DESC desc = {};
			desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
			desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			desc.NodeMask = 0;

			// コマンドキューの生成
			auto hr = Graphics::g_pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(s_pQueue.GetAddressOf()));
			if(FAILED(hr))
			{ return false; }
		}

		// スワップチェインの生成
		{
			// DXGIファクトリーの生成
			ComPtr<IDXGIFactory4> pFactory = nullptr;
			auto hr = CreateDXGIFactory1(IID_PPV_ARGS(&pFactory));
			if(FAILED(hr))
			{ return false; }

			// スワップチェインの設定
			DXGI_SWAP_CHAIN_DESC desc = {};
			desc.BufferDesc.Width = Window::g_Width;
			desc.BufferDesc.Height = Window::g_Height;
			desc.BufferDesc.RefreshRate.Numerator = 60;		// リフレッシュレート
			desc.BufferDesc.RefreshRate.Denominator = 1;
			desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
			desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			desc.BufferCount = FRAME_COUNT;
			desc.OutputWindow = Window::g_hWnd;
			desc.Windowed = TRUE;
			desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

			// スワップチェインの生成
			ComPtr<IDXGISwapChain> pSwapChain = nullptr;
			hr = pFactory->CreateSwapChain(s_pQueue.Get(), &desc, &pSwapChain);

			if(FAILED(hr))
			{ return false; }

			// IDXGISwapChain3を取得
			hr = pSwapChain->QueryInterface(IID_PPV_ARGS(s_pSwapChain.GetAddressOf()));
			if(FAILED(hr))
			{ return false; }

			// バックバッファ番号を取得
			s_FrameIndex = s_pSwapChain->GetCurrentBackBufferIndex();
		}	// スワップチェインの生成

		// RTVヒープの生成
		{
			// ディスクリプタヒープの設定
			D3D12_DESCRIPTOR_HEAP_DESC desc = {};
			desc.NumDescriptors = FRAME_COUNT;	// ダブルバッファ用で2個
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			desc.NodeMask = 0;

			// ディスクリプタヒープを生成
			auto hr = Graphics::g_pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(s_pHeapRTV.GetAddressOf()));
			if(FAILED(hr))
			{ return false; }

			auto handle = s_pHeapRTV->GetCPUDescriptorHandleForHeapStart();
			auto increamentSize =
				Graphics::g_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

			for(auto i = 0u; i < FRAME_COUNT; ++i)
			{
				hr = s_pSwapChain->GetBuffer(i, IID_PPV_ARGS(s_pColorBuffer[i].GetAddressOf()));
				if(FAILED(hr))
				{ return false; }

				D3D12_RENDER_TARGET_VIEW_DESC viewDesc = {};
				viewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	// sRGBで描画表示
				viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;	// バッファにTex2Dとしてアクセス
				viewDesc.Texture2D.MipSlice = 0;
				viewDesc.Texture2D.PlaneSlice = 0;

				// レンダーターゲットビューの生成
				Graphics::g_pDevice->CreateRenderTargetView(s_pColorBuffer[i].Get(), &viewDesc, handle);
				handle.ptr += increamentSize;
			}
		}

		// フェンスの生成
		{
			// フェンスの生成
			auto hr = Graphics::g_pDevice->CreateFence(
				static_cast<UINT64>(0),
				D3D12_FENCE_FLAG_NONE,
				IID_PPV_ARGS(s_pFence.GetAddressOf()));
			if(FAILED(hr))
			{ return false; }
		}

		return true;
	}

	void Terminate(void)
	{
		// GPUの処理の完了を待機
		// シグナル処理
		s_pQueue->Signal(s_pFence.Get(), CURRENT_BUFFER);
		WaitForGpu(s_pFence, CURRENT_BUFFER);

		// フェンス破棄
		s_pFence.Reset();

		// レンダーターゲットビューの破棄
		s_pHeapRTV.Reset();
		for(auto i = 0u; i < FRAME_COUNT; ++i)
		{ s_pColorBuffer[i].Reset(); }

		// スワップチェインの破棄
		s_pSwapChain.Reset();

		// コマンドキューの破棄
		s_pQueue.Reset();
	}

	void Begin(ComPtr<ID3D12GraphicsCommandList> cmdList)
	{
		// コマンドの記録を開始
		Graphics::g_pCmdAllocator[s_FrameIndex]->Reset();
		cmdList->Reset(Graphics::g_pCmdAllocator[s_FrameIndex].Get(), nullptr);

		// リソースバリアの設定
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = s_pColorBuffer[s_FrameIndex].Get();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		// リソースバリア
		cmdList->ResourceBarrier(1, &barrier);

		// レンダーターゲットの設定
		auto handle = s_pHeapRTV->GetCPUDescriptorHandleForHeapStart();
		handle.ptr += (static_cast<uint64_t>(s_FrameIndex)) * Graphics::g_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		cmdList->OMSetRenderTargets(1, &handle, FALSE, nullptr);

		// クリアカラー
		float clearColor[] = { 0.0f,0.0f,1.0f,1.0f };

		// レンダーターゲットビューをクリア
		cmdList->ClearRenderTargetView(handle, clearColor, 0, nullptr);
	}

	void End(ComPtr<ID3D12GraphicsCommandList> cmdList)
	{
		// リソースバリアの設定
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = s_pColorBuffer[s_FrameIndex].Get();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		// リソースバリア
		cmdList->ResourceBarrier(1, &barrier);

		// コマンドの記録を終了
		cmdList->Close();

		// コマンドを実行
		ID3D12CommandList* ppCmdLists[] = { cmdList.Get() };
		s_pQueue->ExecuteCommandLists(1, ppCmdLists);

		// 画面に表示
		Present(1);
	}

	void Present(uint32_t interval)
	{
		// 画面に表示
		s_pSwapChain->Present(interval, 0);

		// シグナル処理
		s_pQueue->Signal(s_pFence.Get(), s_FrameCount);

		// 次のフレームのバッファがまだ書き込み中であれば待機する(ダブルバッファなのでFRAME_COUNT分前のフレームを指定)
		s_FrameCount++;
		WaitForGpu(s_pFence, PREV_BUFFER);

		// バックバッファ番号を更新
		s_FrameIndex = s_pSwapChain->GetCurrentBackBufferIndex();
	}
}