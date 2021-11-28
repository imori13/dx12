#pragma once
#include "WinApp.h"
#include "RenderTargetBuffer.h"
#include "DepthStencilBuffer.h"

namespace Display
{
#define FRAME_COUNT 2

	extern std::array<RenderTargetBuffer, FRAME_COUNT> g_RenderTargetBuffer;
	extern std::array<DepthStencilBuffer, FRAME_COUNT> g_DepthStencilBuffer;
	extern uint32_t g_FrameIndex;

	// ������
	bool Initialize(void);
	// �I��
	void Terminate(void) noexcept;
	// �\������
	void Present(uint32_t interval);
}