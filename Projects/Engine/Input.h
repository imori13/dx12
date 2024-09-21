#pragma once
#include "Vector2.h"

enum class Keys
{
	F1 = DIK_F1,
	F2 = DIK_F2,
	F3 = DIK_F3,
	F4 = DIK_F4,
	F5 = DIK_F5,
	F6 = DIK_F6,
	F7 = DIK_F7,
	F8 = DIK_F8,
	F9 = DIK_F9,
	F10 = DIK_F10,
	F11 = DIK_F11,
	F12 = DIK_F12,

	Num1 = DIK_1,
	Num2 = DIK_2,
	Num3 = DIK_3,
	Num4 = DIK_4,
	Num5 = DIK_5,
	Num6 = DIK_6,
	Num7 = DIK_7,
	Num8 = DIK_8,
	Num9 = DIK_9,
	Num0 = DIK_0,

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

	RightArrow = DIK_RIGHTARROW,
	LeftArrow = DIK_LEFTARROW,
	UpArrow = DIK_UPARROW,
	DownArrow = DIK_DOWNARROW,

	Esc = DIK_ESCAPE,
	BackSpace = DIK_BACKSPACE,
	Space = DIK_SPACE,
	Enter = DIK_RETURN,
	Tab = DIK_TAB,
	CapsLock = DIK_CAPSLOCK,
	LShift = DIK_LSHIFT,
	RShift = DIK_RSHIFT,
	LCtrl = DIK_LCONTROL,
	RCtrl = DIK_RCONTROL,
	LWin = DIK_LWIN,
	RWin = DIK_RWIN,
	LAlt = DIK_LALT,
	RAlt = DIK_RALT,
	LBracket = DIK_LBRACKET,		// "["
	RBracket = DIK_RBRACKET,		// "]"
	Minus = DIK_MINUS,				// "-"
	Comma = DIK_COMMA,				// ","
	Period = DIK_PERIOD,			// "."
	Slash = DIK_SLASH,				// "/"
	BackSlash = DIK_BACKSLASH,		// "\"
	Colon = DIK_COLON,				// ":"
	SemiColon = DIK_SEMICOLON,		// ";"

	Home = DIK_HOME,
	End = DIK_END,
	PageUp = DIK_PGUP,
	PageDown = DIK_PGDN,
	Insert = DIK_INSERT,
	Delete = DIK_DELETE,
	ScrollLock = DIK_SCROLL,
	Pause = DIK_PAUSE,
	Menu = DIK_APPS,

	// 日本語キーボード
	At = DIK_AT,					// "@"
	CirCumflex = DIK_CIRCUMFLEX,	// "^"
	Yen = DIK_YEN,					// "￥"
	Kana = DIK_KANA,				// "ひらがな/カタカナ"
	Kanji = DIK_KANJI,				// "半角/全角|漢字"
	Convert = DIK_CONVERT,			// "変換"
	NoConvert = DIK_NOCONVERT,		// "無変換"

	// USキーボード
	Apostrophe = DIK_APOSTROPHE,	// "'"
	Equals = DIK_EQUALS,			// "="
	Grave = DIK_GRAVE,				// "`"

	// NumPad
	NumEnter = DIK_NUMPADENTER,		// "Numpad Enter"
	NumPadMinus = DIK_NUMPADMINUS,	// "Numpad +"
	NumPadPlus = DIK_NUMPADPLUS,	// "Numpad -"
	NumPeriod = DIK_NUMPADPERIOD,	// "Numpad ."
	NumMultiply = DIK_NUMPADSTAR,	// "Numpad *"
	NumDivide = DIK_NUMPADSLASH,	// "Numpad /"
	NumPad0 = DIK_NUMPAD0,
	NumPad1 = DIK_NUMPAD1,
	NumPad2 = DIK_NUMPAD2,
	NumPad3 = DIK_NUMPAD3,
	NumPad4 = DIK_NUMPAD4,
	NumPad5 = DIK_NUMPAD5,
	NumPad6 = DIK_NUMPAD6,
	NumPad7 = DIK_NUMPAD7,
	NumPad8 = DIK_NUMPAD8,
	NumPad9 = DIK_NUMPAD9,
};

enum class Mouse
{
	Left = 0,
	Right,
	Wheel,
	Other1,
	Other2,
	Other3,
	Other4,
	Other5,
};

namespace Input
{
	void Initialize() noexcept;
	void Terminate() noexcept;
	void Update();

	bool IsKeyHold(Keys keycode) noexcept;
	bool IsKeyDown(Keys keycode) noexcept;
	bool IsKeyUp(Keys keycode) noexcept;

	bool IsMouseDown(Mouse mousecode) noexcept;
	bool IsMouseHold(Mouse mousecode) noexcept;
	bool IsMouseUp(Mouse mousecode) noexcept;

	const Vector2& MousePosition() noexcept;
	const Vector2& MouseVelocity() noexcept;
	float MouseWheelVelocity() noexcept;
};
