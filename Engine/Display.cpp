#include "Display.h"
#include "GraphicsCore.h"
#include <cassert>

namespace
{
	ComPtr<IDXGISwapChain3> s_pSwapChain = nullptr;
	ComPtr<ID3D12Resource> s_pColorBuffer[FRAME_COUNT] = { nullptr };
	ComPtr<ID3D12DescriptorHeap> s_pHeapRTV = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE s_HandleRTV[FRAME_COUNT] = { 0 };

	ComPtr<ID3D12Fence> s_pFence = nullptr;
	HANDLE s_FenceEvent = nullptr;

	ComPtr<ID3D12CommandQueue> s_pQueue = nullptr;	// �ꎞ�I��display�ŋL�q

	uint32_t s_FrameIndex = 0;
	uint64_t s_FenceCounter[FRAME_COUNT] = { 0 };
}

template<typename T>
void SafeRelease(T*& ptr)
{
	if(ptr != nullptr)
	{
		ptr->Release();
		ptr = nullptr;
	}
}

namespace Display
{
	void WaitGpu()
	{
		assert(s_pQueue != nullptr);
		assert(s_pFence != nullptr);
		assert(s_FenceEvent != nullptr);

		// �V�O�i������
		s_pQueue->Signal(s_pFence.Get(), s_FenceCounter[s_FrameIndex]);

		// �������ɃC�x���g��ݒ肷��
		s_pFence->SetEventOnCompletion(s_FenceCounter[s_FrameIndex], s_FenceEvent);

		// �ҋ@����
		WaitForSingleObjectEx(s_FenceEvent, INFINITE, FALSE);

		// �J�E���^�[�𑝂₷
		s_FenceCounter[s_FrameIndex]++;
	}

	bool Initialize(void)
	{
		// �R�}���h�L���[�̐���
		{
			// �R�}���h�L���[�̐ݒ�
			D3D12_COMMAND_QUEUE_DESC desc = {};
			desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
			desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			desc.NodeMask = 0;

			// �R�}���h�L���[�̐���
			auto hr = Graphics::g_pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(s_pQueue.GetAddressOf()));
			if(FAILED(hr))
			{ return false; }
		}

		// �X���b�v�`�F�C���̐���
		{
			// DXGI�t�@�N�g���[�̐���
			IDXGIFactory4* pFactory = nullptr;
			auto hr = CreateDXGIFactory1(IID_PPV_ARGS(&pFactory));
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
			IDXGISwapChain* pSwapChain = nullptr;
			hr = pFactory->CreateSwapChain(s_pQueue.Get(), &desc, &pSwapChain);

			if(FAILED(hr))
			{
				SafeRelease(pFactory);
				return false;
			}

			// IDXGISwapChain3���擾
			hr = pSwapChain->QueryInterface(IID_PPV_ARGS(s_pSwapChain.GetAddressOf()));
			if(FAILED(hr))
			{
				SafeRelease(pFactory);
				SafeRelease(pSwapChain);
				return false;
			}

			// �o�b�N�o�b�t�@�ԍ����擾
			s_FrameIndex = s_pSwapChain->GetCurrentBackBufferIndex();

			// �s�v�ɂȂ����̂ŉ�@
			SafeRelease(pFactory);
			SafeRelease(pSwapChain);
		}	// �X���b�v�`�F�C���̐���

		// RTV�q�[�v�̐���
		{
			// �f�B�X�N���v�^�q�[�v�̐ݒ�
			D3D12_DESCRIPTOR_HEAP_DESC desc = {};
			desc.NumDescriptors = FRAME_COUNT;	// �_�u���o�b�t�@�p��2��
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			desc.NodeMask = 0;

			// �f�B�X�N���v�^�q�[�v�𐶐�
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
				viewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	// sRGB�ŕ`��\��
				viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;	// �o�b�t�@��Tex2D�Ƃ��ăA�N�Z�X
				viewDesc.Texture2D.MipSlice = 0;
				viewDesc.Texture2D.PlaneSlice = 0;

				// �����_�[�^�[�Q�b�g�r���[�̐���
				Graphics::g_pDevice->CreateRenderTargetView(s_pColorBuffer[i].Get(), &viewDesc, handle);
				s_HandleRTV[i] = handle;
				handle.ptr += increamentSize;
			}
		}

		// �t�F���X�̐���
		{
			// �t�F���X�J�E���^�[�����Z�b�g
			for(auto i = 0u; i < FRAME_COUNT; ++i)
			{ s_FenceCounter[i] = 0; }

			// �t�F���X�̐���
			auto hr = Graphics::g_pDevice->CreateFence(
				s_FenceCounter[s_FrameIndex],
				D3D12_FENCE_FLAG_NONE,
				IID_PPV_ARGS(s_pFence.GetAddressOf()));
			if(FAILED(hr))
			{ return false; }

			s_FenceCounter[s_FrameIndex]++;

			// �C�x���g�̐���
			s_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
			if(s_FenceEvent == nullptr)
			{ return false; }
		}
	}

	void Terminate(void)
	{
		// GPU�̏����̊�����ҋ@
		WaitGpu();

		// �C�x���g�j��
		if(s_FenceEvent != nullptr)
		{
			CloseHandle(s_FenceEvent);
			s_FenceEvent = nullptr;
		}

		// �t�F���X�j��
		s_pFence.Reset();

		// �����_�[�^�[�Q�b�g�r���[�̔j��
		s_pHeapRTV.Reset();
		for(auto i = 0u; i < FRAME_COUNT; ++i)
		{ s_pColorBuffer[i].Reset(); }

		// �X���b�v�`�F�C���̔j��
		s_pSwapChain.Reset();

		// �R�}���h�L���[�̔j��
		s_pQueue.Reset();
	}

	void Begin(ComPtr<ID3D12GraphicsCommandList> cmdList)
	{
		// �R�}���h�̋L�^���J�n
		Graphics::g_pCmdAllocator[s_FrameIndex]->Reset();
		cmdList->Reset(Graphics::g_pCmdAllocator[s_FrameIndex].Get(), nullptr);

		// ���\�[�X�o���A�̐ݒ�
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = s_pColorBuffer[s_FrameIndex].Get();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		// ���\�[�X�o���A
		cmdList->ResourceBarrier(1, &barrier);

		// �����_�[�^�[�Q�b�g�̐ݒ�
		cmdList->OMSetRenderTargets(1, &s_HandleRTV[s_FrameIndex], FALSE, nullptr);

		// �N���A�J���[
		float clearColor[] = { 0.0f,0.0f,1.0f,1.0f };

		// �����_�[�^�[�Q�b�g�r���[���N���A
		cmdList->ClearRenderTargetView(s_HandleRTV[s_FrameIndex], clearColor, 0, nullptr);
	}

	void End(ComPtr<ID3D12GraphicsCommandList> cmdList)
	{
		// ���\�[�X�o���A�̐ݒ�
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = s_pColorBuffer[s_FrameIndex].Get();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		// ���\�[�X�o���A
		cmdList->ResourceBarrier(1, &barrier);

		// �R�}���h�̋L�^���I��
		cmdList->Close();

		// �R�}���h�����s
		ID3D12CommandList* ppCmdLists[] = { cmdList.Get() };
		s_pQueue->ExecuteCommandLists(1, ppCmdLists);

		// ��ʂɕ\��
		Present(1);
	}

	void Present(uint32_t interval)
	{
		// ��ʂɕ\��
		s_pSwapChain->Present(interval, 0);

		// �V�O�i������
		const auto currentValue = s_FenceCounter[s_FrameIndex];
		s_pQueue->Signal(s_pFence.Get(), currentValue);

		// �o�b�N�o�b�t�@�ԍ����X�V
		s_FrameIndex = s_pSwapChain->GetCurrentBackBufferIndex();

		// ���̃t���[���̕`�揀�����܂��ł���Αҋ@����
		if(s_pFence->GetCompletedValue() < s_FenceCounter[s_FrameIndex])
		{
			s_pFence->SetEventOnCompletion(s_FenceCounter[s_FrameIndex], s_FenceEvent);
			WaitForSingleObjectEx(s_FenceEvent, INFINITE, FALSE);
		}

		// ���̃t���[���̃t�F���X�J�E���^�[�𑝂₷
		s_FenceCounter[s_FrameIndex] = currentValue + 1;
	}
}