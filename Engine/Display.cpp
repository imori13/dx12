#include "Display.h"
#include "GraphicsCore.h"
#include <cassert>

namespace
{
	ComPtr<IDXGISwapChain3> s_pSwapChain = nullptr;
	ComPtr<ID3D12Resource> s_pColorBuffer[FRAME_COUNT] = { nullptr };
	ComPtr<ID3D12DescriptorHeap> s_pHeapRTV = nullptr;

	uint32_t s_FrameIndex = 0;
}

namespace Display
{
	bool Initialize(void)
	{
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
			hr = pFactory->CreateSwapChain(Graphics::g_pCmdQueue.Get(), &desc, &pSwapChain);

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

		return true;
	}

	void Terminate(void)
	{
		Graphics::IdleGpu();

		// レンダーターゲットビューの破棄
		s_pHeapRTV.Reset();
		for(auto i = 0u; i < FRAME_COUNT; ++i)
		{ s_pColorBuffer[i].Reset(); }

		// スワップチェインの破棄
		s_pSwapChain.Reset();
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

		// コマンドを実行
		auto nextFrame = Graphics::ExecuteCommandList(cmdList);

		Graphics::WaitForFence(nextFrame);

		// 画面に表示
		Present(1);
	}

	void Present(uint32_t interval)
	{
		// 画面に表示
		s_pSwapChain->Present(interval, 0);

		// バックバッファ番号を更新
		s_FrameIndex = s_pSwapChain->GetCurrentBackBufferIndex();
	}
}