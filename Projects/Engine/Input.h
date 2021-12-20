#pragma once
#include "Vector2.h"

enum class Mouse
{
	LeftButton,
	RightButton,
	WheelButton,
};

namespace Input
{
	void Initialize() noexcept;
	void Update() noexcept;

	bool IsLeft() noexcept;
	bool IsLeftDown() noexcept;
	bool IsLeftUp() noexcept;

	bool IsRight() noexcept;
	bool IsRightDown() noexcept;
	bool IsRightUp() noexcept;

	bool IsWheel() noexcept;
	bool IsWheelDown() noexcept;
	bool IsWheelUp() noexcept;
	const Vector2& MousePos() noexcept;

	extern LRESULT CALLBACK ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam) noexcept(false);
};
