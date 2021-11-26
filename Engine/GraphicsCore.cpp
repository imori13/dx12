#include "GraphicsCore.h"
#include "Display.h"

namespace Graphics
{
	ComPtr<ID3D12Device> g_pDevice = nullptr;
	ComPtr<ID3D12Resource> g_pColorBuffer[FRAME_COUNT] = { nullptr };
	Command g_Command;

	bool Initialize()
	{
		HRESULT hr;

		// �f�o�b�O���C���[�̗L����
#if defined(DEBUG) || defined(_DEBUG)
		{
			ComPtr<ID3D12Debug> debug;
			hr = D3D12GetDebugInterface(IID_PPV_ARGS(debug.GetAddressOf()));

			// �f�o�b�O���C���[��L����
			if(SUCCEEDED(hr))
			{ debug->EnableDebugLayer(); }
		}
#endif

		// �f�o�C�X�̐���
		hr = D3D12CreateDevice(
			nullptr,
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(g_pDevice.GetAddressOf()));
		if(FAILED(hr))
		{ return false; }

		g_Command.Create(FRAME_COUNT);

		return true;
	}

	void Terminate()
	{
		// �J���[�o�b�t�@�̔j��
		for(auto i = 0u; i < FRAME_COUNT; ++i)
		{ g_pColorBuffer[i].Reset(); }

		// �R�}���h�̔j��
		g_Command.Destroy();

		// �f�o�C�X�̔j��
		g_pDevice.Reset();
	}
}