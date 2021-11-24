#include "HogeApp.h"
#include "WinApp.h"
#include <cassert>

namespace
{
	template<typename T>
	void SafeRelease(T*& ptr)
	{
		if(ptr != nullptr)
		{
			ptr->Release();
			ptr = nullptr;
		}
	}
}

HogeApp::HogeApp()
	: m_pDevice(nullptr)
	, m_pQueue(nullptr)
	, m_pSwapChain(nullptr)
	, m_pCmdList(nullptr)
	, m_pHeapRTV(nullptr)
	, m_pFence(nullptr)
	, m_FrameIndex(0)
{
	for(auto i = 0u; i < FrameCount; ++i)
	{
		m_pColorBuffer[i] = nullptr;
		m_pCmdAllocator[i] = nullptr;
		m_FenceCounter[i] = 0;
	}
}

HogeApp::~HogeApp()
{
}

bool HogeApp::InitApp()
{
	// Direct3D12の初期化
	if(!InitD3D())
	{ return false; }

	// 正常終了
	return true;
}

void HogeApp::TermApp()
{
	// Direct3D12の終了処理
	TermD3D();
}

bool HogeApp::InitD3D()
{
	// デバッグレイヤーの有効化
#if defined(DEBUG) || defined(_DEBUG)
	{
		ComPtr<ID3D12Debug> debug;
		auto hr = D3D12GetDebugInterface(IID_PPV_ARGS(debug.GetAddressOf()));

		// デバッグレイヤーを有効化
		if(SUCCEEDED(hr))
		{ debug->EnableDebugLayer(); }
	}
#endif

	// ■ デバイスの生成
	auto hr = D3D12CreateDevice(
		nullptr,
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(m_pDevice.GetAddressOf()));
	if(FAILED(hr))
	{ return false; }

	// ■ コマンドキューの生成
	{
		// コマンドキューの設定
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask = 0;

		// コマンドキューの生成
		hr = m_pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(m_pQueue.GetAddressOf()));
		if(FAILED(hr))
		{ return false; }
	}

	// ■ スワップチェインの生成
	{
		// DXGIファクトリーの生成
		IDXGIFactory4* pFactory = nullptr;
		hr = CreateDXGIFactory1(IID_PPV_ARGS(&pFactory));
		if(FAILED(hr))
		{ return false; }

		// スワップチェインの設定
		DXGI_SWAP_CHAIN_DESC desc = {};
		desc.BufferDesc.Width = Window::g_Width;	// 解像度の横幅
		desc.BufferDesc.Height = Window::g_Height;	// 解像度の縦幅
		desc.BufferDesc.RefreshRate.Numerator = 60;		// リフレッシュレート(分子)
		desc.BufferDesc.RefreshRate.Denominator = 1;	// リフレッシュレート(分母)
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;	// 走査線描画モード
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;	// 拡大縮小の設定
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// 表示形式のピクセルフォーマット
		desc.SampleDesc.Count = 1;	// マルチサンプリングの設定
		desc.SampleDesc.Quality = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // バックバッファの使用方法
		desc.BufferCount = FrameCount;	// バックバッファの数を指定
		desc.OutputWindow = Window::g_hWnd;	// 出力するウィンドウへのハンドル
		desc.Windowed = TRUE;	// ?
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	// バックバッファ入れ替え時の効果を指定
		// Present()時にバックバッファの内容が破棄される。
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// よくわからない

		// スワップチェインの生成
		IDXGISwapChain* pSwapChain = nullptr;
		hr = pFactory->CreateSwapChain(m_pQueue.Get(), &desc, &pSwapChain);

		if(FAILED(hr))
		{
			SafeRelease(pFactory);
			return false;
		}

		// IDXGISwapChain3を取得
		hr = pSwapChain->QueryInterface(IID_PPV_ARGS(m_pSwapChain.GetAddressOf()));
		if(FAILED(hr))
		{
			SafeRelease(pFactory);
			SafeRelease(pSwapChain);
			return false;
		}

		// バックバッファ番号を取得
		m_FrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

		// 不要になったので解法
		SafeRelease(pFactory);
		SafeRelease(pSwapChain);
	}

	// ■ コマンドアロケータの生成
	{
		for(auto i = 0u; i < FrameCount; ++i)
		{
			hr = m_pDevice->CreateCommandAllocator(
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				IID_PPV_ARGS(m_pCmdAllocator[i].GetAddressOf()));	// ダブルバッファリング用で2個
			if(FAILED(hr))
			{ return false; }
		}
	}

	// ■ コマンドリストの生成
	{
		hr = m_pDevice->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			m_pCmdAllocator[m_FrameIndex].Get(),
			nullptr,	// パイプラインステート(後で設定する)
			IID_PPV_ARGS(m_pCmdList.GetAddressOf()));
		if(FAILED(hr))
		{ return false; }
	}

	// ■ レンダーターゲットビューの生成
	{
		// ディスクリプタヒープの設定
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.NumDescriptors = FrameCount;	// ダブルバッファ用で2個
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		desc.NodeMask = 0;

		// ディスクリプタヒープを生成
		hr = m_pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_pHeapRTV.GetAddressOf()));
		if(FAILED(hr))
		{ return false; }

		auto handle = m_pHeapRTV->GetCPUDescriptorHandleForHeapStart();
		auto increamentSize =
			m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		for(auto i = 0u; i < FrameCount; ++i)
		{
			hr = m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(m_pColorBuffer[i].GetAddressOf()));
			if(FAILED(hr))
			{ return false; }

			D3D12_RENDER_TARGET_VIEW_DESC viewDesc = {};
			viewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	// sRGBで描画表示
			viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;	// バッファにTex2Dとしてアクセス
			viewDesc.Texture2D.MipSlice = 0;
			viewDesc.Texture2D.PlaneSlice = 0;

			// レンダーターゲットビューの生成
			m_pDevice->CreateRenderTargetView(m_pColorBuffer[i].Get(), &viewDesc, handle);
			m_HandleRTV[i] = handle;
			handle.ptr += increamentSize;
		}
	}

	// フェンスの生成
	{
		// フェンスカウンターをリセット
		for(auto i = 0u; i < FrameCount; ++i)
		{ m_FenceCounter[i] = 0; }

		// フェンスの生成
		hr = m_pDevice->CreateFence(
			m_FenceCounter[m_FrameIndex],
			D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(m_pFence.GetAddressOf()));
		if(FAILED(hr))
		{ return false; }

		m_FenceCounter[m_FrameIndex]++;

		// イベントの生成
		m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if(m_FenceEvent == nullptr)
		{ return false; }
	}

	// コマンドリストを閉じる
	m_pCmdList->Close();

	return true;
}

void HogeApp::Render()
{
	// コマンドの記録を開始
	m_pCmdAllocator[m_FrameIndex]->Reset();
	m_pCmdList->Reset(m_pCmdAllocator[m_FrameIndex].Get(), nullptr);

	// リソースバリアの設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = m_pColorBuffer[m_FrameIndex].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	// リソースバリア
	m_pCmdList->ResourceBarrier(1, &barrier);

	// レンダーターゲットの設定
	m_pCmdList->OMSetRenderTargets(1, &m_HandleRTV[m_FrameIndex], FALSE, nullptr);

	// クリアカラー
	float clearColor[] = { 0.0f,0.0f,1.0f,1.0f };

	// レンダーターゲットビューをクリア
	m_pCmdList->ClearRenderTargetView(m_HandleRTV[m_FrameIndex], clearColor, 0, nullptr);

	// 描画処理
	{
		// TODO : ポリゴン描画用の処理を追加
	}

	// リソースバリアの設定
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = m_pColorBuffer[m_FrameIndex].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	// リソースバリア
	m_pCmdList->ResourceBarrier(1, &barrier);

	// コマンドの記録を終了
	m_pCmdList->Close();

	// コマンドを実行
	ID3D12CommandList* ppCmdLists[] = { m_pCmdList.Get() };
	m_pQueue->ExecuteCommandLists(1, ppCmdLists);

	// 画面に表示
	Present(1);
}

void HogeApp::Present(uint32_t interval)
{
	// 画面に表示
	m_pSwapChain->Present(interval, 0);

	// シグナル処理
	const auto currentValue = m_FenceCounter[m_FrameIndex];
	m_pQueue->Signal(m_pFence.Get(), currentValue);

	// バックバッファ番号を更新
	m_FrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	// 次のフレームの描画準備がまだであれば待機する
	if(m_pFence->GetCompletedValue() < m_FenceCounter[m_FrameIndex])
	{
		m_pFence->SetEventOnCompletion(m_FenceCounter[m_FrameIndex], m_FenceEvent);
		WaitForSingleObjectEx(m_FenceEvent, INFINITE, FALSE);
	}

	// 次のフレームのフェンスカウンターを増やす
	m_FenceCounter[m_FrameIndex] = currentValue + 1;
}

void HogeApp::TermD3D()
{
	// GPUの処理の完了を待機
	WaitGpu();

	// イベント破棄
	if(m_FenceEvent != nullptr)
	{
		CloseHandle(m_FenceEvent);
		m_FenceEvent = nullptr;

		// フェンス破棄
		m_pFence.Reset();

		// レンダーターゲットビューの破棄
		m_pHeapRTV.Reset();
		for(auto i = 0u; i < FrameCount; ++i)
		{ m_pColorBuffer[i].Reset(); }

		// コマンドリストの破棄
		m_pCmdList.Reset();

		// コマンドアロケータの破棄
		for(auto i = 0u; i < FrameCount; ++i)
		{ m_pCmdAllocator[i].Reset(); }

		// スワップチェインの破棄
		m_pSwapChain.Reset();

		// コマンドキューの破棄
		m_pQueue.Reset();

		// デバイスの破棄
		m_pDevice.Reset();
	}
}

void HogeApp::WaitGpu()
{
	assert(m_pQueue != nullptr);
	assert(m_pFence != nullptr);
	assert(m_FenceEvent != nullptr);

	// シグナル処理
	m_pQueue->Signal(m_pFence.Get(), m_FenceCounter[m_FrameIndex]);

	// 完了時にイベントを設定する
	m_pFence->SetEventOnCompletion(m_FenceCounter[m_FrameIndex], m_FenceEvent);

	// 待機処理
	WaitForSingleObjectEx(m_FenceEvent, INFINITE, FALSE);

	// カウンターを増やす
	m_FenceCounter[m_FrameIndex]++;
}
