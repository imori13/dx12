#pragma once

namespace Timer
{
	extern float g_ElapsedTime;
	extern float g_FrameTime;

	void Initialize() noexcept(false);
	void Update() noexcept(false);
};

