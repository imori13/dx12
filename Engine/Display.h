#pragma once
#include "WinApp.h"
#include "RenderTargetBuffer.h"
#include "DepthStencilBuffer.h"

namespace Display
{
#define FRAME_COUNT 2

	extern RenderTargetBuffer g_RenderTargetBuffer[FRAME_COUNT];
	extern DepthStencilBuffer g_DepthStencilBuffer[FRAME_COUNT];
	extern uint32_t g_FrameIndex;

	// ‰Šú‰»
	bool Initialize(void);
	// I—¹
	void Terminate(void);
	// •\¦ˆ—
	void Present(uint32_t interval);
}