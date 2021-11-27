#pragma once
#include "WinApp.h"
#include "RTVBuffer.h"
#include "DepthStencilBuffer.h"

namespace Display
{
#define FRAME_COUNT 2

	extern RTVBuffer g_RtvBuffer[FRAME_COUNT];
	extern DepthStencilBuffer g_DepthStencilBuffer[FRAME_COUNT];
	extern uint32_t g_FrameIndex;

	// 初期化
	bool Initialize(void);
	// 終了
	void Terminate(void);
	// 表示処理
	void Present(uint32_t interval);
}