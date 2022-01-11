#include "Timer.h"

namespace
{
	LARGE_INTEGER s_Frequency;
	LARGE_INTEGER s_StartTimer;
	LARGE_INTEGER s_EndTimer;
}

namespace Timer
{
	float g_ElapsedTime;
	float g_FrameTime;

	void Initialize()
	{
		QueryPerformanceFrequency(&s_Frequency);
		QueryPerformanceCounter(&s_StartTimer);
	}

	void Update()
	{
		QueryPerformanceCounter(&s_EndTimer);
		const float NewElapsed = static_cast<float>(s_EndTimer.QuadPart - s_StartTimer.QuadPart) / static_cast<float>(s_Frequency.QuadPart);
		g_FrameTime = NewElapsed - g_ElapsedTime;
		g_ElapsedTime = NewElapsed;
	}
}