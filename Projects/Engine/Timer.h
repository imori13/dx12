#pragma once

namespace Timer
{
	extern double g_ElapsedTime;
	extern float g_FrameTime;

	void Initialize() noexcept(false);
	void Update(const bool frameWait) noexcept(false);
};

