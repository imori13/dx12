#pragma once
#include "WinApp.h"

namespace Display
{
#define FRAME_COUNT 2

	extern uint32_t g_FrameIndex;

	// ������
	bool Initialize(void);
	// �I��
	void Terminate(void);
	// �\������
	void Present(uint32_t interval);
}