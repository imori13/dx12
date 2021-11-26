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

		// デバッグレイヤーの有効化
#if defined(DEBUG) || defined(_DEBUG)
		{
			ComPtr<ID3D12Debug> debug;
			hr = D3D12GetDebugInterface(IID_PPV_ARGS(debug.GetAddressOf()));

			// デバッグレイヤーを有効化
			if(SUCCEEDED(hr))
			{ debug->EnableDebugLayer(); }
		}
#endif

		// デバイスの生成
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
		// カラーバッファの破棄
		for(auto i = 0u; i < FRAME_COUNT; ++i)
		{ g_pColorBuffer[i].Reset(); }

		// コマンドの破棄
		g_Command.Destroy();

		// デバイスの破棄
		g_pDevice.Reset();
	}
}