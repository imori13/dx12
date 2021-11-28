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
	std::array<RenderTargetBuffer, FRAME_COUNT> g_RenderTargetBuffer;
	std::array<DepthStencilBuffer, FRAME_COUNT> g_DepthStencilBuffer;

	bool Initialize(void)
	{
		HRESULT hr = 0;

		// �X���b�v�`�F�C���̐���
		{
			// DXGI�t�@�N�g���[�̐���
			ComPtr<IDXGIFactory4> pFactory = nullptr;
			hr = CreateDXGIFactory1(IID_PPV_ARGS(&pFactory));
			if(FAILED(hr))
			{ return false; }

			// �X���b�v�`�F�C���̐ݒ�
			DXGI_SWAP_CHAIN_DESC desc = {};
			desc.BufferDesc.Width = Window::g_Width;
			desc.BufferDesc.Height = Window::g_Height;
			desc.BufferDesc.RefreshRate.Numerator = 60;		// ���t���b�V�����[�g
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

			// �X���b�v�`�F�C���̐���
			ComPtr<IDXGISwapChain> pSwapChain = nullptr;
			hr = pFactory->CreateSwapChain(Graphics::g_Command.GetCmdQueue(), &desc, &pSwapChain);

			if(FAILED(hr))
			{ return false; }

			// IDXGISwapChain3���擾
			hr = pSwapChain->QueryInterface(IID_PPV_ARGS(s_pSwapChain.GetAddressOf()));
			if(FAILED(hr))
			{ return false; }

			// �o�b�N�o�b�t�@�ԍ����擾
			g_FrameIndex = s_pSwapChain->GetCurrentBackBufferIndex();
		}

		// �����_�[�^�[�Q�b�g�r���[�̐���
		s_RenderTargetHeap.Create(gsl::narrow<uint32_t>(g_RenderTargetBuffer.size()), D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		for(auto i = 0u; i < g_RenderTargetBuffer.size(); ++i)
		{
			hr = s_pSwapChain->GetBuffer(i, IID_PPV_ARGS(g_RenderTargetBuffer.at(i).GetAddressOf()));
			if(FAILED(hr))
			{ return false; }

			// �����_�[�^�[�Q�b�g�r���[�̐���
			const auto& rtvView = g_RenderTargetBuffer.at(i).GetView();
			const auto handle = s_RenderTargetHeap.GetHandle(i);
			Graphics::g_pDevice->CreateRenderTargetView(g_RenderTargetBuffer.at(i).Get(), &rtvView, handle);
			g_RenderTargetBuffer.at(i).SetCpuHandle(handle);
		}

		// �f�t�X�X�e���V���r���[�̐���
		s_DepthStencilHeap.Create(gsl::narrow<uint32_t>(g_DepthStencilBuffer.size()), D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		for(auto& itr : g_DepthStencilBuffer)
		{
			itr.Create(DXGI_FORMAT_D32_FLOAT, 1.0f, 0);

			const auto& dsvView = itr.GetView();
			const auto handle = s_DepthStencilHeap.GetHandle();
			Graphics::g_pDevice->CreateDepthStencilView(itr.Get(), &dsvView, handle);
			itr.SetCpuHandle(handle);
		}

		return true;
	}

	void Terminate(void) noexcept
	{
	}

	void Present(uint32_t interval)
	{
		// ��ʂɕ\��
		s_pSwapChain->Present(interval, 0);

		// �o�b�N�o�b�t�@�ԍ����X�V
		g_FrameIndex = s_pSwapChain->GetCurrentBackBufferIndex();
	}
}