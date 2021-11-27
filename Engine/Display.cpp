#include "Display.h"
#include "GraphicsCore.h"
#include "PixelHeap.h"

namespace
{
	ComPtr<IDXGISwapChain3> s_pSwapChain = nullptr;
	PixelHeap s_RenderTargetHeap;
	PixelHeap s_DepthStencilHeap;
}

namespace Display
{
	uint32_t g_FrameIndex = 0;
	RenderTargetBuffer g_RenderTargetBuffer[FRAME_COUNT];
	DepthStencilBuffer g_DepthStencilBuffer[FRAME_COUNT];

	bool Initialize(void)
	{
		HRESULT hr = 0;

		// スワップチェインの生成
		{
			// DXGIファクトリーの生成
			ComPtr<IDXGIFactory4> pFactory = nullptr;
			hr = CreateDXGIFactory1(IID_PPV_ARGS(&pFactory));
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
			hr = pFactory->CreateSwapChain(Graphics::g_Command.GetCmdQueue(), &desc, &pSwapChain);

			if(FAILED(hr))
			{ return false; }

			// IDXGISwapChain3を取得
			hr = pSwapChain->QueryInterface(IID_PPV_ARGS(s_pSwapChain.GetAddressOf()));
			if(FAILED(hr))
			{ return false; }

			// バックバッファ番号を取得
			g_FrameIndex = s_pSwapChain->GetCurrentBackBufferIndex();
		}

		// レンダーターゲットビューの生成
		const uint32_t BUFFER_COUNT = FRAME_COUNT;
		s_RenderTargetHeap.Create(BUFFER_COUNT, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		for(auto i = 0u; i < BUFFER_COUNT; ++i)
		{
			hr = s_pSwapChain->GetBuffer(i, IID_PPV_ARGS(g_RenderTargetBuffer[i].GetAddressOf()));
			if(FAILED(hr))
			{ return false; }

			// レンダーターゲットビューの生成
			const auto& rtvView = g_RenderTargetBuffer[i].GetView();
			auto handle = s_RenderTargetHeap.GetHandle(i);
			Graphics::g_pDevice->CreateRenderTargetView(g_RenderTargetBuffer[i].Get(), &rtvView, handle);
			g_RenderTargetBuffer[i].SetCpuHandle(handle);
		}

		// デフスステンシルビューの生成
		s_DepthStencilHeap.Create(BUFFER_COUNT, D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		for(auto i = 0u; i < BUFFER_COUNT; ++i)
		{
			g_DepthStencilBuffer[i].Create(DXGI_FORMAT_D32_FLOAT, 1.0f, 0);

			const auto& dsvView = g_DepthStencilBuffer->GetView();
			auto handle = s_DepthStencilHeap.GetHandle();
			Graphics::g_pDevice->CreateDepthStencilView(g_DepthStencilBuffer[i].Get(), &dsvView, handle);
			g_DepthStencilBuffer[i].SetCpuHandle(handle);
		}

		return true;
	}

	void Terminate(void)
	{
		s_RenderTargetHeap.Destroy();
		s_DepthStencilHeap.Destroy();

		g_RenderTargetBuffer->Destroy();
		g_DepthStencilBuffer->Destroy();

		// スワップチェインの破棄
		s_pSwapChain.Reset();
	}

	void Present(uint32_t interval)
	{
		// 画面に表示
		s_pSwapChain->Present(interval, 0);

		// バックバッファ番号を更新
		g_FrameIndex = s_pSwapChain->GetCurrentBackBufferIndex();
	}
}