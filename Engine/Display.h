#pragma once
#include "WinApp.h"

namespace Display
{
#define FRAME_COUNT 2

	extern uint32_t s_FrameIndex;

	// 初期化
	bool Initialize(void);
	// 終了
	void Terminate(void);
	// 描画受付
	void Begin();
	// 描画終了
	void End();
	// 表示処理
	void Present(uint32_t interval);
}