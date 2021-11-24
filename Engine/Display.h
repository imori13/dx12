#pragma once
#include "WinApp.h"

using namespace Microsoft::WRL;

namespace Display
{
#define FRAME_COUNT 2

	// 初期化
	bool Initialize(void);
	// 終了
	void Terminate(void);
	// 描画受付
	void Begin(ComPtr<ID3D12GraphicsCommandList> cmdList);
	// 描画終了
	void End(ComPtr<ID3D12GraphicsCommandList> cmdList);
	// 表示処理
	void Present(uint32_t interval);
}