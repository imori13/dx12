#include "GraphicsCore.h"

namespace Graphics
{
	ComPtr<ID3D12CommandAllocator> g_pCmdAllocator[FRAME_COUNT] = { nullptr };
	ComPtr<ID3D12Device> g_pDevice = nullptr;
	ComPtr<ID3D12GraphicsCommandList> g_pCmdList = nullptr;

	bool Initialize()
	{
		// デバッグレイヤーの有効化
#if defined(DEBUG) || defined(_DEBUG)
		{
			ComPtr<ID3D12Debug> debug;
			auto hr = D3D12GetDebugInterface(IID_PPV_ARGS(debug.GetAddressOf()));

			// デバッグレイヤーを有効化
			if(SUCCEEDED(hr))
			{ debug->EnableDebugLayer(); }
		}
#endif

		// デバイスの生成
		auto hr = D3D12CreateDevice(
			nullptr,
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(g_pDevice.GetAddressOf()));
		if(FAILED(hr))
		{ return false; }

		// コマンドアロケータの生成
		{
			for(auto i = 0u; i < FRAME_COUNT; ++i)
			{
				hr = g_pDevice->CreateCommandAllocator(
					D3D12_COMMAND_LIST_TYPE_DIRECT,
					IID_PPV_ARGS(g_pCmdAllocator[i].GetAddressOf()));	// ダブルバッファリング用で2個
				if(FAILED(hr))
				{ return false; }
			}
		}

		// コマンドリストの生成
		{
			hr = g_pDevice->CreateCommandList(
				0,
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				g_pCmdAllocator[0].Get(),
				nullptr,	// パイプラインステート(後で設定する)
				IID_PPV_ARGS(g_pCmdList.GetAddressOf()));
			if(FAILED(hr))
			{ return false; }
		}

		g_pCmdList->Close();

		return true;
	}

	void Terminate()
	{
		// コマンドリストの破棄
		g_pCmdList.Reset();

		// コマンドアロケータの破棄
		for(auto i = 0u; i < FRAME_COUNT; ++i)
		{ g_pCmdAllocator[i].Reset(); }

		// デバイスの破棄
		g_pDevice.Reset();
	}
}