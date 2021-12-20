#include "Input.h"
#include "WinApp.h"

struct MouseState
{
	bool LeftButton;
	bool RightButton;
	bool WheelButton;
	float WheelValue;
};

namespace
{
	// マウス用
	Vector2 s_MousePos;
	MouseState s_State;
	MouseState s_PrevMouseState;
	MouseState s_CurrentMouseState;

	// キーボード用
	IDirectInputDevice8* devkeyboard;
	std::array<BYTE, 256> currentKey;
	std::array<BYTE, 256> prevKey;
}

namespace Input
{
	void Initialize() noexcept
	{
		IDirectInput8* dinput = nullptr;
		auto result = DirectInput8Create(
			WinApp::g_hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, reinterpret_cast<void**>(&dinput), nullptr);
		result = dinput->CreateDevice(GUID_SysKeyboard, &devkeyboard, nullptr);
		result = devkeyboard->SetDataFormat(&c_dfDIKeyboard);
		result = devkeyboard->SetCooperativeLevel(WinApp::g_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	}

	void Update() noexcept
	{
		s_PrevMouseState = s_CurrentMouseState;
		s_CurrentMouseState = s_State;

		// 1フレーム前のキー状態を保存
		for(int i = 0; i < currentKey.size(); i++)
		{
			prevKey.at(i) = currentKey.at(i);
		}
		auto result = devkeyboard->Acquire();
		result = devkeyboard->GetDeviceState(currentKey.size() * sizeof(BYTE), currentKey.data());
	}

	bool IsKey(KeyCode keycode) noexcept
	{
		return currentKey.at(static_cast<uint8_t>(keycode));
	}
	bool IsKeyDown(KeyCode keycode) noexcept
	{
		return currentKey.at(static_cast<uint8_t>(keycode)) && !prevKey.at(static_cast<uint8_t>(keycode));
	}
	bool IsKeyUp(KeyCode keycode) noexcept
	{
		return !currentKey.at(static_cast<uint8_t>(keycode)) && prevKey.at(static_cast<uint8_t>(keycode));
	}

	bool IsLeft() noexcept
	{
		return s_CurrentMouseState.LeftButton;
	}
	bool IsLeftDown() noexcept
	{
		return s_CurrentMouseState.LeftButton && !s_PrevMouseState.LeftButton;
	}
	bool IsLeftUp() noexcept
	{
		return !s_CurrentMouseState.LeftButton && s_PrevMouseState.LeftButton;
	}

	bool IsRight() noexcept
	{
		return s_CurrentMouseState.RightButton;
	}
	bool IsRightDown() noexcept
	{
		return s_CurrentMouseState.RightButton && !s_PrevMouseState.RightButton;
	}
	bool IsRightUp() noexcept
	{
		return !s_CurrentMouseState.RightButton && s_PrevMouseState.RightButton;
	}

	float GetWheelValue() noexcept
	{
		return s_CurrentMouseState.WheelValue / WHEEL_DELTA;
	}

	bool IsWheel() noexcept
	{
		return s_CurrentMouseState.WheelButton;
	}
	bool IsWheelDown() noexcept
	{
		return s_CurrentMouseState.WheelButton && !s_PrevMouseState.WheelButton;
	}
	bool IsWheelUp() noexcept
	{
		return !s_CurrentMouseState.WheelButton && s_PrevMouseState.WheelButton;
	}

	const Vector2& MousePos() noexcept
	{
		return s_MousePos;
	}

	LRESULT CALLBACK ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam) noexcept(false)
	{
		const float x = LOWORD(lParam);
		const float y = HIWORD(lParam);
		constexpr float limit = 10000;
		s_MousePos.x() = (x >= limit) ? (0) : (x);
		s_MousePos.y() = (y >= limit) ? (0) : (y);
		s_State.WheelValue = GET_WHEEL_DELTA_WPARAM(wParam);

		switch(message)
		{
			// 左クリック
			case WM_LBUTTONDOWN:
			{
				s_State.LeftButton = true;
				break;
			}
			case WM_LBUTTONUP:
			{
				s_State.LeftButton = false;
				break;
			}

			// 右クリック
			case WM_RBUTTONDOWN:
			{
				s_State.RightButton = true;
				break;
			}
			case WM_RBUTTONUP:
			{
				s_State.RightButton = false;
				break;
			}

			// ホイールクリック
			case WM_MBUTTONDOWN:
			{
				s_State.WheelButton = true;
				break;
			}
			case WM_MBUTTONUP:
			{
				s_State.WheelButton = false;
				break;
			}
			// ホイール
			case WM_MOUSEWHEEL:
			{
				//s_State.WheelValue = GET_WHEEL_DELTA_WPARAM(wParam);
				break;
			}

			default:
			{
				break;
			}
		}

		return true;
	}
}
