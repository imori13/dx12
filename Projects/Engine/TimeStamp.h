#pragma once

namespace TimeStamp
{
	void Begin() noexcept;
	float End() noexcept;

	void Stop() noexcept;
	void Resume() noexcept;
}