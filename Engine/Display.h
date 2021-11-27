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

	// ������
	bool Initialize(void);
	// �I��
	void Terminate(void);
	// �\������
	void Present(uint32_t interval);
}