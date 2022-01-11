#include "GraphicsCore.h"
#include "Display.h"
#include "Debug.h"

namespace Graphics
{
	Microsoft::WRL::ComPtr<ID3D12Device> g_pDevice = nullptr;

	void Initialize()
	{
		HRESULT hr{};

		// デバッグレイヤーの有効化
#if defined(DEBUG) || defined(_DEBUG)
		{
			Microsoft::WRL::ComPtr<ID3D12Debug> debug = nullptr;
			hr = D3D12GetDebugInterface(IID_PPV_ARGS(debug.GetAddressOf()));

			// デバッグレイヤーを有効化
			ENSURES(hr, "DebugLayer有効化");

			debug->EnableDebugLayer();
		}
#endif
		// デバイスの生成
		hr = D3D12CreateDevice(
			nullptr,
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(g_pDevice.GetAddressOf()));
		ENSURES(hr, "Device生成");
	}

	void Terminate() noexcept
	{

	}
}