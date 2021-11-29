#include "GraphicsCore.h"
#include "Display.h"

namespace Graphics
{
	ComPtr<ID3D12Device> g_pDevice = nullptr;
	ComPtr<ID3D12Resource> g_pColorBuffer[FRAME_COUNT] = { nullptr };
	Command g_Command;

	void Initialize()
	{
		HRESULT hr{};

		// �f�o�b�O���C���[�̗L����
#if defined(DEBUG) || defined(_DEBUG)
		{
			ComPtr<ID3D12Debug> debug = nullptr;
			hr = D3D12GetDebugInterface(IID_PPV_ARGS(debug.GetAddressOf()));

			// �f�o�b�O���C���[��L����
			ENSURES(hr, "DebugLayer�L����");

			debug->EnableDebugLayer();
		}
#endif
		// �f�o�C�X�̐���
		hr = D3D12CreateDevice(
			nullptr,
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(g_pDevice.GetAddressOf()));
		ENSURES(hr, "Device����");

		// �R�}���h�̐���
		g_Command.Create(FRAME_COUNT);
	}

	void Terminate() noexcept
	{

	}
}