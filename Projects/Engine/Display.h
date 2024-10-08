#pragma once
#include "RenderTargetBuffer.h"
#include "DepthStencilBuffer.h"

constexpr uint32_t FRAME_COUNT = 2;

namespace Display
{
	extern std::array<RenderTargetBuffer, FRAME_COUNT> g_RenderTargetBuffer;
	extern std::array<DepthStencilBuffer, FRAME_COUNT> g_DepthStencilBuffer;
	extern uint32_t g_FrameIndex;
	extern uint32_t g_AppWidth;
	extern uint32_t g_AppHeight;
	extern float g_Aspect;

	extern D3D12_VIEWPORT g_Viewport;
	extern D3D12_RECT g_Scissor;

	// 初期化
	void Initialize(void);
	// 終了
	void Terminate(void) noexcept;
	// 表示処理
	void Present(uint32_t interval);

	void UpdateNextFrame();
	void OnSizeChanged(uint32_t width, uint32_t height);
	void ToggleFullscreen();
}