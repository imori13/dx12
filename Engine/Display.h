#pragma once
#include "RenderTargetBuffer.h"
#include "DepthStencilBuffer.h"

#define FRAME_COUNT 2

namespace Display
{
	extern std::array<RenderTargetBuffer, FRAME_COUNT> g_RenderTargetBuffer;
	extern std::array<DepthStencilBuffer, FRAME_COUNT> g_DepthStencilBuffer;
	extern uint32_t g_FrameIndex;

	// 初期化
	void Initialize(void);
	// 終了
	void Terminate(void) noexcept;
	// 表示処理
	void Present(uint32_t interval);
}