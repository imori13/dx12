#pragma once
#include "Vector2.h"
#include <dinput.h>

enum class Key
{
	Num0 = DIK_0,
	Num1 = DIK_1,
	Num2 = DIK_2,
	Num3 = DIK_3,
	Num4 = DIK_4,
	Num5 = DIK_5,
	Num6 = DIK_6,
	Num7 = DIK_7,
	Num8 = DIK_8,
	Num9 = DIK_9,
	A = DIK_A,
	B = DIK_B,
	C = DIK_C,
	D = DIK_D,
	E = DIK_E,
	F = DIK_F,
	G = DIK_G,
	H = DIK_H,
	I = DIK_I,
	J = DIK_J,
	K = DIK_K,
	L = DIK_L,
	M = DIK_M,
	N = DIK_N,
	O = DIK_O,
	P = DIK_P,
	Q = DIK_Q,
	R = DIK_R,
	S = DIK_S,
	T = DIK_T,
	U = DIK_U,
	V = DIK_V,
	W = DIK_W,
	X = DIK_X,
	Y = DIK_Y,
	Z = DIK_Z,
	Esc = DIK_ESCAPE,
	Space = DIK_SPACE,
	Right = DIK_RIGHTARROW,
	Left = DIK_LEFTARROW,
	Up = DIK_UPARROW,
	Down = DIK_DOWNARROW,
	NumMinus = DIK_SUBTRACT,
	NumPlus = DIK_ADD,
	LAlt = DIK_LALT,
	RAlt = DIK_RALT,
	LShift = DIK_LSHIFT,
	Enter = DIK_RETURN,
	Back = DIK_BACK,
};

enum class Mouse
{
	Left = 0,
	Right = 1,
	Wheel = 2,
};

namespace Input
{
	void Initialize() noexcept;
	void Update();

	bool IsKeyHold(Key keycode) noexcept;
	bool IsKeyDown(Key keycode) noexcept;
	bool IsKeyUp(Key keycode) noexcept;

	bool IsMouseDown(Mouse mouseCode) noexcept;
	bool IsMouseHold(Mouse mouseCode) noexcept;
	bool IsMouseUp(Mouse mouseCode) noexcept;

	const Vector2& MousePosition() noexcept;
	const Vector2& MouseVelocity() noexcept;
	float MouseWheelVelocity() noexcept;
};
