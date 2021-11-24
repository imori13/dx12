#include "GraphicsCore.h"

namespace Graphics
{
	ComPtr<ID3D12CommandAllocator> g_pCmdAllocator[FRAME_COUNT] = { nullptr };
	ComPtr<ID3D12Device> g_pDevice = nullptr;
	ComPtr<ID3D12GraphicsCommandList> g_pCmdList = nullptr;

	bool Initialize()
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

		// �f�o�C�X�̐���
		auto hr = D3D12CreateDevice(
			nullptr,
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(g_pDevice.GetAddressOf()));
		if(FAILED(hr))
		{ return false; }

		// �R�}���h�A���P�[�^�̐���
		{
			for(auto i = 0u; i < FRAME_COUNT; ++i)
			{
				hr = g_pDevice->CreateCommandAllocator(
					D3D12_COMMAND_LIST_TYPE_DIRECT,
					IID_PPV_ARGS(g_pCmdAllocator[i].GetAddressOf()));	// �_�u���o�b�t�@�����O�p��2��
				if(FAILED(hr))
				{ return false; }
			}
		}

		// �R�}���h���X�g�̐���
		{
			hr = g_pDevice->CreateCommandList(
				0,
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				g_pCmdAllocator[0].Get(),
				nullptr,	// �p�C�v���C���X�e�[�g(��Őݒ肷��)
				IID_PPV_ARGS(g_pCmdList.GetAddressOf()));
			if(FAILED(hr))
			{ return false; }
		}

		g_pCmdList->Close();

		return true;
	}

	void Terminate()
	{
		// �R�}���h���X�g�̔j��
		g_pCmdList.Reset();

		// �R�}���h�A���P�[�^�̔j��
		for(auto i = 0u; i < FRAME_COUNT; ++i)
		{ g_pCmdAllocator[i].Reset(); }

		// �f�o�C�X�̔j��
		g_pDevice.Reset();
	}
}