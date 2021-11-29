#pragma once
#include "RenderTargetBuffer.h"
#include "DepthStencilBuffer.h"

#define FRAME_COUNT 2

namespace Display
{
	extern std::array<RenderTargetBuffer, FRAME_COUNT> g_RenderTargetBuffer;
	extern std::array<DepthStencilBuffer, FRAME_COUNT> g_DepthStencilBuffer;
	extern uint32_t g_FrameIndex;

	// ‰Šú‰»
	void Initialize(void);
	// I—¹
	void Terminate(void) noexcept;
	// •\¦ˆ—
	void Present(uint32_t interval);
}