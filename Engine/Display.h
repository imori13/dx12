#pragma once
#include "WinApp.h"

namespace Display
{
#define FRAME_COUNT 2

	extern uint32_t g_FrameIndex;

	// ‰Šú‰»
	bool Initialize(void);
	// I—¹
	void Terminate(void);
	// •\¦ˆ—
	void Present(uint32_t interval);
}