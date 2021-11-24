#pragma once
#include "WinApp.h"

namespace Display
{
#define FRAME_COUNT 2

	extern uint32_t s_FrameIndex;

	// ������
	bool Initialize(void);
	// �I��
	void Terminate(void);
	// �`���t
	void Begin();
	// �`��I��
	void End();
	// �\������
	void Present(uint32_t interval);
}