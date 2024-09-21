#include "Display.h"
#include "GraphicsCore.h"
#include "ResourceHeap.h"
#include "WinApp.h"
#include "Command.h"
#include "Debug.h"

namespace
{
	Microsoft::WRL::ComPtr<IDXGISwapChain3> s_pSwapChain = nullptr;
	ResourceHeap s_RenderTargetHeap;
	ResourceHeap s_DepthStencilHeap;
	bool s_IsFullscreen{};
	bool s_TearingSupport{};

	constexpr uint32_t s_RefreshRate = 120;
}

namespace Display
{
	uint32_t g_FrameIndex = 0;
	std::array<RenderTargetBuffer, FRAME_COUNT> g_RenderTargetBuffer;
	std::array<DepthStencilBuffer, FRAME_COUNT> g_DepthStencilBuffer;
	uint32_t g_AppWidth;
	uint32_t g_AppHeight;
	float g_Aspect;
	D3D12_VIEWPORT g_Viewport;
	D3D12_RECT g_Scissor;

	void CreateColorBuffer();
	void InitializeViewport(uint32_t width, uint32_t height) noexcept;

	void Initialize(void)
	{
		HRESULT hr{};

		// スワップチェインの生成
		{
			// DXGIファクトリーの生成
			Microsoft::WRL::ComPtr<IDXGIFactory4> pFactory = nullptr;
			hr = CreateDXGIFactory1(IID_PPV_ARGS(&pFactory));
			ENSURES(hr, "DXGIFactory生成");

			BOOL allowTearing = FALSE;
			if(SUCCEEDED(hr))
			{
				Microsoft::WRL::ComPtr<IDXGIFactory5> factory5;
				hr = pFactory.As(&factory5);
				if(SUCCEEDED(hr))
				{
					hr = factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing));
				}
			}
			s_TearingSupport = SUCCEEDED(hr) && allowTearing;

			g_AppWidth = WinApp::g_Width;
			g_AppHeight = WinApp::g_Height;

			// スワップチェインの設定
			DXGI_SWAP_CHAIN_DESC desc = {};
			desc.Windowed = !s_IsFullscreen;
			desc.BufferDesc.Width = g_AppWidth;
			desc.BufferDesc.Height = g_AppHeight;
			desc.BufferDesc.RefreshRate.Numerator = s_RefreshRate;		// リフレッシュレート
			desc.BufferDesc.RefreshRate.Denominator = 1;
			desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
			desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			desc.BufferCount = FRAME_COUNT;
			desc.OutputWindow = WinApp::g_hWnd;
			desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			//desc.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
			desc.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
			desc.Flags |= s_TearingSupport ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

			// スワップチェインの生成
			Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain = nullptr;
			hr = pFactory->CreateSwapChain(Command::GetCmdQueue(), &desc, &pSwapChain);
			ENSURES(hr, "SwapChain生成");

			// DXGIのAlt+Enterを禁止する(CreateSwapchainの後に呼ぶ)
			hr = pFactory->MakeWindowAssociation(WinApp::g_hWnd, DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER);
			ENSURES(hr, "DXGIfullscreenn無効化");

			// IDXGISwapChain3を取得
			hr = pSwapChain->QueryInterface(IID_PPV_ARGS(s_pSwapChain.GetAddressOf()));
			ENSURES(hr, "SwapChain取得");

			// バックバッファ番号を取得
			g_FrameIndex = s_pSwapChain->GetCurrentBackBufferIndex();
		}

		// RSV/DSVの生成
		s_RenderTargetHeap.Create(gsl::narrow<uint32_t>(g_RenderTargetBuffer.size()), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
		s_DepthStencilHeap.Create(gsl::narrow<uint32_t>(g_DepthStencilBuffer.size()), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

		CreateColorBuffer();
		InitializeViewport(g_AppWidth, g_AppHeight);
	}

	void Terminate(void) noexcept
	{
	}

	void Present(uint32_t interval)
	{
		const uint32_t presentFlags = (interval == 0 && s_TearingSupport && !s_IsFullscreen) ? DXGI_PRESENT_ALLOW_TEARING : 0;

		// 画面に表示
		const auto hr = s_pSwapChain->Present(interval, presentFlags);
		ENSURES(hr);
	}

	void UpdateNextFrame()
	{
		g_FrameIndex = s_pSwapChain->GetCurrentBackBufferIndex();
	}

	void OnSizeChanged(uint32_t width, uint32_t height)
	{
		Command::WaitForGpu();

		// リソースのクリア
		for(auto i = 0u; i < FRAME_COUNT; ++i)
		{
			g_RenderTargetBuffer.at(i).Reset();
			g_DepthStencilBuffer.at(i).Reset();
		}

		// スワップチェインをリサイズ
		DXGI_SWAP_CHAIN_DESC desc = {};
		auto hr = s_pSwapChain->GetDesc(&desc);
		ENSURES(hr, "Swapchain設定の読み込み");
		hr = s_pSwapChain->ResizeBuffers(FRAME_COUNT, width, height, desc.BufferDesc.Format, desc.Flags);
		ENSURES(hr, "Swapchainリサイズバッファ");

		Command::MoveToNextFrame();

		// サイズ変更
		g_AppWidth = width;
		g_AppHeight = height;

		CreateColorBuffer();
		InitializeViewport(width, height);
	}

	void ToggleFullscreen()
	{
		LOGLINE("fullscreen切り替え");

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc;
		s_pSwapChain->GetFullscreenDesc(&fullscreenDesc);

		// Fullscreen -> Windowed
		if(!fullscreenDesc.Windowed)
		{
			s_pSwapChain->SetFullscreenState(false, nullptr);
			SetWindowLong(WinApp::g_hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
			ShowWindow(WinApp::g_hWnd, SW_NORMAL);
		}
		// Windowed -> Fullscreen
		else
		{
			DXGI_MODE_DESC desc{};
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.Width = 1920;
			desc.Height = 1080;
			desc.RefreshRate.Denominator = 1;
			desc.RefreshRate.Numerator = s_RefreshRate;
			desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
			s_pSwapChain->ResizeTarget(&desc);
			s_pSwapChain->SetFullscreenState(true, nullptr);
		}

		// 解像度を変更する
		//OnSizeChanged(WinApp::g_Width, WinApp::g_Height);

		s_IsFullscreen = !s_IsFullscreen;
	}

	void CreateColorBuffer()
	{
		s_RenderTargetHeap.InitializeIndex();
		s_DepthStencilHeap.InitializeIndex();

		for(auto i = 0u; i < FRAME_COUNT; ++i)
		{
			const auto hr = s_pSwapChain->GetBuffer(i, IID_PPV_ARGS(g_RenderTargetBuffer.at(i).GetAddressOf()));
			ENSURES(hr, "SwapChainのBuffer取得");

			// レンダーターゲットビューの生成
			const auto& rtvView = g_RenderTargetBuffer.at(i).GetView();
			const auto handle = s_RenderTargetHeap.GetNextHandle();
			Graphics::g_pDevice->CreateRenderTargetView(g_RenderTargetBuffer.at(i).Get(), &rtvView, handle.CPU);
			g_RenderTargetBuffer.at(i).SetCpuHandle(handle.CPU);
		}
		for(auto i = 0u; i < g_DepthStencilBuffer.size(); ++i)
		{
			g_DepthStencilBuffer.at(i).Create(Display::g_AppWidth, Display::g_AppHeight, DXGI_FORMAT_D32_FLOAT, 1.0f, 0);

			const auto& dsvView = g_DepthStencilBuffer.at(i).GetView();
			const auto handle = s_DepthStencilHeap.GetNextHandle();
			Graphics::g_pDevice->CreateDepthStencilView(g_DepthStencilBuffer.at(i).Get(), &dsvView, handle.CPU);
			g_DepthStencilBuffer.at(i).SetCpuHandle(handle.CPU);
		}
	}

	void InitializeViewport(uint32_t width, uint32_t height) noexcept
	{
		g_Viewport.TopLeftX = 0;
		g_Viewport.TopLeftY = 0;
		g_Viewport.Width = gsl::narrow<float>(width);
		g_Viewport.Height = gsl::narrow<float>(height);
		g_Viewport.MinDepth = 0.0f;
		g_Viewport.MaxDepth = 1.0f;
		g_Scissor.left = 0;
		g_Scissor.right = width;
		g_Scissor.top = 0;
		g_Scissor.bottom = height;

		g_Aspect = static_cast<float>(Display::g_AppWidth) / static_cast<float>(Display::g_AppHeight);
	}
}