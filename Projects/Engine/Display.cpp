#include "Display.h"
#include "GraphicsCore.h"
#include "ResourceHeap.h"
#include "WinApp.h"
#include "Command.h"

namespace
{
	Microsoft::WRL::ComPtr<IDXGISwapChain3> s_pSwapChain = nullptr;
	ResourceHeap s_RenderTargetHeap;
	ResourceHeap s_DepthStencilHeap;
}

namespace Display
{
	uint32_t g_FrameIndex = 0;
	std::array<RenderTargetBuffer, FRAME_COUNT> g_RenderTargetBuffer;
	std::array<DepthStencilBuffer, FRAME_COUNT> g_DepthStencilBuffer;
	uint32_t g_AppWidth;
	uint32_t g_AppHeight;

	void CreateRenderTargetView();

	void Initialize(void)
	{
		HRESULT hr{};

		// スワップチェインの生成
		{
			// DXGIファクトリーの生成
			Microsoft::WRL::ComPtr<IDXGIFactory4> pFactory = nullptr;
			hr = CreateDXGIFactory1(IID_PPV_ARGS(&pFactory));
			ENSURES(hr, "DXGIFactory生成");

			g_AppWidth = Window::g_Width;
			g_AppHeight = Window::g_Height;

			// スワップチェインの設定
			DXGI_SWAP_CHAIN_DESC desc = {};
			desc.BufferDesc.Width = g_AppWidth;
			desc.BufferDesc.Height = g_AppHeight;
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
			Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain = nullptr;
			hr = pFactory->CreateSwapChain(Command::GetCmdQueue(), &desc, &pSwapChain);
			ENSURES(hr, "SwapChain生成");

			// IDXGISwapChain3を取得
			hr = pSwapChain->QueryInterface(IID_PPV_ARGS(s_pSwapChain.GetAddressOf()));
			ENSURES(hr, "SwapChain取得");

			// バックバッファ番号を取得
			g_FrameIndex = s_pSwapChain->GetCurrentBackBufferIndex();
		}

		// レンダーターゲットビューの生成
		s_RenderTargetHeap.Create(gsl::narrow<uint32_t>(g_RenderTargetBuffer.size()), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
		CreateRenderTargetView();

		// デフスステンシルビューの生成
		s_DepthStencilHeap.Create(gsl::narrow<uint32_t>(g_DepthStencilBuffer.size()), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
		for(auto i = 0u; i < g_DepthStencilBuffer.size(); ++i)
		{
			g_DepthStencilBuffer.at(i).Create(Display::g_AppWidth, Display::g_AppHeight, DXGI_FORMAT_D32_FLOAT, 1.0f, 0);

			const auto& dsvView = g_DepthStencilBuffer.at(i).GetView();
			const auto handle = s_DepthStencilHeap.GetCPUHandle(i);
			Graphics::g_pDevice->CreateDepthStencilView(g_DepthStencilBuffer.at(i).Get(), &dsvView, handle);
			g_DepthStencilBuffer.at(i).SetCpuHandle(handle);
		}
	}

	void Terminate(void) noexcept
	{
	}

	void Present(uint32_t interval)
	{
		// 画面に表示
		s_pSwapChain->Present(interval, 0);
	}

	void NextFrame()
	{
		g_FrameIndex = s_pSwapChain->GetCurrentBackBufferIndex();
	}

	void OnSizeChanged(uint32_t width, uint32_t height)
	{
		if(width == Window::g_Width || height == Window::g_Height)return;

		Command::WaitForGpu();

		// リソースのクリア
		for(auto i = 0u; i < FRAME_COUNT; ++i)
		{ g_RenderTargetBuffer.at(i).Get()->Release(); }

		// スワップチェインをリサイズ
		DXGI_SWAP_CHAIN_DESC desc = {};
		s_pSwapChain->GetDesc(&desc);
		s_pSwapChain->ResizeBuffers(FRAME_COUNT, width, height, desc.BufferDesc.Format, desc.Flags);

		BOOL fullScreenState;
		s_pSwapChain->GetFullscreenState(&fullScreenState, nullptr);

		g_FrameIndex = s_pSwapChain->GetCurrentBackBufferIndex();

		// サイズ変更
		g_AppWidth = width;
		g_AppHeight = height;
	}

	void CreateRenderTargetView()
	{
		for(auto i = 0u; i < g_RenderTargetBuffer.size(); ++i)
		{
			const auto hr = s_pSwapChain->GetBuffer(i, IID_PPV_ARGS(g_RenderTargetBuffer.at(i).GetAddressOf()));
			ENSURES(hr, "SwapChainのBuffer取得");

			// レンダーターゲットビューの生成
			const auto& rtvView = g_RenderTargetBuffer.at(i).GetView();
			const auto handle = s_RenderTargetHeap.GetCPUHandle(i);
			Graphics::g_pDevice->CreateRenderTargetView(g_RenderTargetBuffer.at(i).Get(), &rtvView, handle);
			g_RenderTargetBuffer.at(i).SetCpuHandle(handle);
		}
	}
}