#include "Input.h"

struct MouseState
{
	bool LeftButton;
	bool RightButton;
	bool WheelButton;
	float WheelValue;
};

namespace
{
	Vector2 s_MousePos;
	MouseState s_State;
	MouseState s_PrevMouseState;
	MouseState s_CurrentMouseState;
}

namespace Input
{
	void Initialize() noexcept
	{
	}

	void Update() noexcept
	{
		s_PrevMouseState = s_CurrentMouseState;
		s_CurrentMouseState = s_State;
	}

	bool IsLeft() noexcept
	{
		return s_CurrentMouseState.LeftButton;
	}
	bool IsLeftDown() noexcept
	{
		return (s_CurrentMouseState.LeftButton == true && s_PrevMouseState.LeftButton == false);
	}
	bool IsLeftUp() noexcept
	{
		return (s_CurrentMouseState.LeftButton == false && s_PrevMouseState.LeftButton == true);
	}

	bool IsRight() noexcept
	{
		return s_CurrentMouseState.RightButton;
	}
	bool IsRightDown() noexcept
	{
		return (s_CurrentMouseState.RightButton == true && s_PrevMouseState.RightButton == false);
	}
	bool IsRightUp() noexcept
	{
		return (s_CurrentMouseState.RightButton == false && s_PrevMouseState.RightButton == true);
	}

	bool IsWheel() noexcept
	{
		return s_CurrentMouseState.WheelButton;
	}
	bool IsWheelDown() noexcept
	{
		return (s_CurrentMouseState.WheelButton == true && s_PrevMouseState.WheelButton == false);
	}
	bool IsWheelUp() noexcept
	{
		return (s_CurrentMouseState.WheelButton == false && s_PrevMouseState.WheelButton == true);
	}

	const Vector2& MousePos() noexcept
	{
		return s_MousePos;
	}

	LRESULT CALLBACK ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam) noexcept(false)
	{
		s_MousePos.x() = LOWORD(lParam);
		s_MousePos.y() = HIWORD(lParam);

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
				s_State.WheelValue = GET_WHEEL_DELTA_WPARAM(wParam);
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
