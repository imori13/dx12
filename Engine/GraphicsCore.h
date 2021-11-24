#pragma once
#include "Display.h"

using namespace Microsoft::WRL;

namespace Graphics
{
	extern ComPtr<ID3D12Device> g_pDevice;					// デバイス
	extern ComPtr<ID3D12CommandAllocator> g_pCmdAllocator[FRAME_COUNT];
	extern ComPtr<ID3D12GraphicsCommandList> g_pCmdList;	// コマンドリスト

	bool Initialize();
	void Terminate();
}