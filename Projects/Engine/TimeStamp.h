#pragma once

namespace TimeStamp
{
	void Begin();
	float End() noexcept;

	void Stop() noexcept;
	void Resume() noexcept;
}