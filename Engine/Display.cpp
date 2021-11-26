#include "Display.h"
#include "GraphicsCore.h"
#include "RTVHeap.h"

namespace
{
	ComPtr<IDXGISwapChain3> s_pSwapChain = nullptr;
}

namespace Display
{
	uint32_t g_FrameIndex = 0;

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

		auto handle = Graphics::g_RtvHeap.GetCpuHandle();
		for(auto i = 0u; i < FRAME_COUNT; ++i)
		{
			hr = s_pSwapChain->GetBuffer(i, IID_PPV_ARGS(Graphics::g_pColorBuffer[i].GetAddressOf()));
			if(FAILED(hr))
			{ return false; }

			D3D12_RENDER_TARGET_VIEW_DESC viewDesc = {};
			viewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	// sRGBで描画表示
			viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;	// バッファにTex2Dとしてアクセス
			viewDesc.Texture2D.MipSlice = 0;
			viewDesc.Texture2D.PlaneSlice = 0;

			// レンダーターゲットビューの生成
			Graphics::g_pDevice->CreateRenderTargetView(Graphics::g_pColorBuffer[i].Get(), &viewDesc, handle);
			handle.ptr += Graphics::g_RtvHeap.GetIncrementSize();
		}

		return true;
	}

	void Terminate(void)
	{
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