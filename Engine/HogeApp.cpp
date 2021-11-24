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
	// Direct3D12�̏�����
	if(!InitD3D())
	{ return false; }

	// ����I��
	return true;
}

void HogeApp::TermApp()
{
	// Direct3D12�̏I������
	TermD3D();
}

bool HogeApp::InitD3D()
{
	// �f�o�b�O���C���[�̗L����
#if defined(DEBUG) || defined(_DEBUG)
	{
		ComPtr<ID3D12Debug> debug;
		auto hr = D3D12GetDebugInterface(IID_PPV_ARGS(debug.GetAddressOf()));

		// �f�o�b�O���C���[��L����
		if(SUCCEEDED(hr))
		{ debug->EnableDebugLayer(); }
	}
#endif

	// �� �f�o�C�X�̐���
	auto hr = D3D12CreateDevice(
		nullptr,
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(m_pDevice.GetAddressOf()));
	if(FAILED(hr))
	{ return false; }

	// �� �R�}���h�L���[�̐���
	{
		// �R�}���h�L���[�̐ݒ�
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask = 0;

		// �R�}���h�L���[�̐���
		hr = m_pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(m_pQueue.GetAddressOf()));
		if(FAILED(hr))
		{ return false; }
	}

	// �� �X���b�v�`�F�C���̐���
	{
		// DXGI�t�@�N�g���[�̐���
		IDXGIFactory4* pFactory = nullptr;
		hr = CreateDXGIFactory1(IID_PPV_ARGS(&pFactory));
		if(FAILED(hr))
		{ return false; }

		// �X���b�v�`�F�C���̐ݒ�
		DXGI_SWAP_CHAIN_DESC desc = {};
		desc.BufferDesc.Width = Window::g_Width;	// �𑜓x�̉���
		desc.BufferDesc.Height = Window::g_Height;	// �𑜓x�̏c��
		desc.BufferDesc.RefreshRate.Numerator = 60;		// ���t���b�V�����[�g(���q)
		desc.BufferDesc.RefreshRate.Denominator = 1;	// ���t���b�V�����[�g(����)
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;	// �������`�惂�[�h
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;	// �g��k���̐ݒ�
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// �\���`���̃s�N�Z���t�H�[�}�b�g
		desc.SampleDesc.Count = 1;	// �}���`�T���v�����O�̐ݒ�
		desc.SampleDesc.Quality = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // �o�b�N�o�b�t�@�̎g�p���@
		desc.BufferCount = FrameCount;	// �o�b�N�o�b�t�@�̐����w��
		desc.OutputWindow = Window::g_hWnd;	// �o�͂���E�B���h�E�ւ̃n���h��
		desc.Windowed = TRUE;	// ?
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	// �o�b�N�o�b�t�@����ւ����̌��ʂ��w��
		// Present()���Ƀo�b�N�o�b�t�@�̓��e���j�������B
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// �悭�킩��Ȃ�

		// �X���b�v�`�F�C���̐���
		IDXGISwapChain* pSwapChain = nullptr;
		hr = pFactory->CreateSwapChain(m_pQueue.Get(), &desc, &pSwapChain);

		if(FAILED(hr))
		{
			SafeRelease(pFactory);
			return false;
		}

		// IDXGISwapChain3���擾
		hr = pSwapChain->QueryInterface(IID_PPV_ARGS(m_pSwapChain.GetAddressOf()));
		if(FAILED(hr))
		{
			SafeRelease(pFactory);
			SafeRelease(pSwapChain);
			return false;
		}

		// �o�b�N�o�b�t�@�ԍ����擾
		m_FrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

		// �s�v�ɂȂ����̂ŉ�@
		SafeRelease(pFactory);
		SafeRelease(pSwapChain);
	}

	// �� �R�}���h�A���P�[�^�̐���
	{
		for(auto i = 0u; i < FrameCount; ++i)
		{
			hr = m_pDevice->CreateCommandAllocator(
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				IID_PPV_ARGS(m_pCmdAllocator[i].GetAddressOf()));	// �_�u���o�b�t�@�����O�p��2��
			if(FAILED(hr))
			{ return false; }
		}
	}

	// �� �R�}���h���X�g�̐���
	{
		hr = m_pDevice->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			m_pCmdAllocator[m_FrameIndex].Get(),
			nullptr,	// �p�C�v���C���X�e�[�g(��Őݒ肷��)
			IID_PPV_ARGS(m_pCmdList.GetAddressOf()));
		if(FAILED(hr))
		{ return false; }
	}

	// �� �����_�[�^�[�Q�b�g�r���[�̐���
	{
		// �f�B�X�N���v�^�q�[�v�̐ݒ�
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.NumDescriptors = FrameCount;	// �_�u���o�b�t�@�p��2��
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		desc.NodeMask = 0;

		// �f�B�X�N���v�^�q�[�v�𐶐�
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
			viewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	// sRGB�ŕ`��\��
			viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;	// �o�b�t�@��Tex2D�Ƃ��ăA�N�Z�X
			viewDesc.Texture2D.MipSlice = 0;
			viewDesc.Texture2D.PlaneSlice = 0;

			// �����_�[�^�[�Q�b�g�r���[�̐���
			m_pDevice->CreateRenderTargetView(m_pColorBuffer[i].Get(), &viewDesc, handle);
			m_HandleRTV[i] = handle;
			handle.ptr += increamentSize;
		}
	}

	// �t�F���X�̐���
	{
		// �t�F���X�J�E���^�[�����Z�b�g
		for(auto i = 0u; i < FrameCount; ++i)
		{ m_FenceCounter[i] = 0; }

		// �t�F���X�̐���
		hr = m_pDevice->CreateFence(
			m_FenceCounter[m_FrameIndex],
			D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(m_pFence.GetAddressOf()));
		if(FAILED(hr))
		{ return false; }

		m_FenceCounter[m_FrameIndex]++;

		// �C�x���g�̐���
		m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if(m_FenceEvent == nullptr)
		{ return false; }
	}

	// �R�}���h���X�g�����
	m_pCmdList->Close();

	return true;
}

void HogeApp::Render()
{
	// �R�}���h�̋L�^���J�n
	m_pCmdAllocator[m_FrameIndex]->Reset();
	m_pCmdList->Reset(m_pCmdAllocator[m_FrameIndex].Get(), nullptr);

	// ���\�[�X�o���A�̐ݒ�
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = m_pColorBuffer[m_FrameIndex].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	// ���\�[�X�o���A
	m_pCmdList->ResourceBarrier(1, &barrier);

	// �����_�[�^�[�Q�b�g�̐ݒ�
	m_pCmdList->OMSetRenderTargets(1, &m_HandleRTV[m_FrameIndex], FALSE, nullptr);

	// �N���A�J���[
	float clearColor[] = { 0.0f,0.0f,1.0f,1.0f };

	// �����_�[�^�[�Q�b�g�r���[���N���A
	m_pCmdList->ClearRenderTargetView(m_HandleRTV[m_FrameIndex], clearColor, 0, nullptr);

	// �`�揈��
	{
		// TODO : �|���S���`��p�̏�����ǉ�
	}

	// ���\�[�X�o���A�̐ݒ�
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = m_pColorBuffer[m_FrameIndex].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	// ���\�[�X�o���A
	m_pCmdList->ResourceBarrier(1, &barrier);

	// �R�}���h�̋L�^���I��
	m_pCmdList->Close();

	// �R�}���h�����s
	ID3D12CommandList* ppCmdLists[] = { m_pCmdList.Get() };
	m_pQueue->ExecuteCommandLists(1, ppCmdLists);

	// ��ʂɕ\��
	Present(1);
}

void HogeApp::Present(uint32_t interval)
{
	// ��ʂɕ\��
	m_pSwapChain->Present(interval, 0);

	// �V�O�i������
	const auto currentValue = m_FenceCounter[m_FrameIndex];
	m_pQueue->Signal(m_pFence.Get(), currentValue);

	// �o�b�N�o�b�t�@�ԍ����X�V
	m_FrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	// ���̃t���[���̕`�揀�����܂��ł���Αҋ@����
	if(m_pFence->GetCompletedValue() < m_FenceCounter[m_FrameIndex])
	{
		m_pFence->SetEventOnCompletion(m_FenceCounter[m_FrameIndex], m_FenceEvent);
		WaitForSingleObjectEx(m_FenceEvent, INFINITE, FALSE);
	}

	// ���̃t���[���̃t�F���X�J�E���^�[�𑝂₷
	m_FenceCounter[m_FrameIndex] = currentValue + 1;
}

void HogeApp::TermD3D()
{
	// GPU�̏����̊�����ҋ@
	WaitGpu();

	// �C�x���g�j��
	if(m_FenceEvent != nullptr)
	{
		CloseHandle(m_FenceEvent);
		m_FenceEvent = nullptr;

		// �t�F���X�j��
		m_pFence.Reset();

		// �����_�[�^�[�Q�b�g�r���[�̔j��
		m_pHeapRTV.Reset();
		for(auto i = 0u; i < FrameCount; ++i)
		{ m_pColorBuffer[i].Reset(); }

		// �R�}���h���X�g�̔j��
		m_pCmdList.Reset();

		// �R�}���h�A���P�[�^�̔j��
		for(auto i = 0u; i < FrameCount; ++i)
		{ m_pCmdAllocator[i].Reset(); }

		// �X���b�v�`�F�C���̔j��
		m_pSwapChain.Reset();

		// �R�}���h�L���[�̔j��
		m_pQueue.Reset();

		// �f�o�C�X�̔j��
		m_pDevice.Reset();
	}
}

void HogeApp::WaitGpu()
{
	assert(m_pQueue != nullptr);
	assert(m_pFence != nullptr);
	assert(m_FenceEvent != nullptr);

	// �V�O�i������
	m_pQueue->Signal(m_pFence.Get(), m_FenceCounter[m_FrameIndex]);

	// �������ɃC�x���g��ݒ肷��
	m_pFence->SetEventOnCompletion(m_FenceCounter[m_FrameIndex], m_FenceEvent);

	// �ҋ@����
	WaitForSingleObjectEx(m_FenceEvent, INFINITE, FALSE);

	// �J�E���^�[�𑝂₷
	m_FenceCounter[m_FrameIndex]++;
}
