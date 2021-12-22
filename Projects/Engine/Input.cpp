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
	IDirectInput8* s_InputInterface;

	// キーボード用
	IDirectInputDevice8* s_KeyboardDevice;
	std::array<BYTE, 256> currentKey;
	std::array<BYTE, 256> prevKey;

	// マウス用
	IDirectInputDevice8* s_MouseDevice;
	Vector2 s_MousePosition;
	Vector2 s_MouseVelocity;
	float s_MouseVelocityZ;
	DIMOUSESTATE prevMouse;
	DIMOUSESTATE currentMouse;
}

namespace
{
	void UpdateKeyboard() noexcept
	{
		// キーボード状態取得
		{
			prevKey = currentKey;
			auto hr = s_KeyboardDevice->Acquire();
			hr = s_KeyboardDevice->GetDeviceState(currentKey.size() * sizeof(BYTE), currentKey.data());
		}
	}

	void UpdateMouse()
	{
		// マウス状態取得
		{
			prevMouse = currentMouse;
			auto hr = s_MouseDevice->Acquire();
			hr = s_MouseDevice->GetDeviceState(sizeof(DIMOUSESTATE), &currentMouse);
		}

		// マウス座標取得
		{
			POINT p;
			GetCursorPos(&p);
			ScreenToClient(WinApp::g_hWnd, &p);
			s_MousePosition = Vector2(static_cast<float>(p.x), static_cast<float>(p.y));
		}

		// マウス移動量取得
		{
			constexpr float Normalize = 1.0f / 120.0f;
			s_MouseVelocity = Vector2(static_cast<float>(currentMouse.lX), static_cast<float>(currentMouse.lY));
			s_MouseVelocityZ = static_cast<float>(currentMouse.lZ) * Normalize;
		}
	}

	constexpr uint32_t MOUSE_ON_VALUE = 0x80;
	constexpr inline bool IsClick(const DIMOUSESTATE& state, Mouse mouse)
	{
		return gsl::at(state.rgbButtons, static_cast<uint32_t>(mouse)) & MOUSE_ON_VALUE;
	}

	constexpr inline bool IsKey(gsl::span<BYTE> span, Key key)
	{
		return span[static_cast<uint32_t>(key)];
	}
}

namespace Input
{
	void Initialize() noexcept
	{
		auto hr = DirectInput8Create(WinApp::g_hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, reinterpret_cast<void**>(&s_InputInterface), nullptr);
		ENSURES(hr);

		// キーボード
		{
			hr = s_InputInterface->CreateDevice(GUID_SysKeyboard, &s_KeyboardDevice, nullptr);
			ENSURES(hr);
			hr = s_KeyboardDevice->SetDataFormat(&c_dfDIKeyboard);
			ENSURES(hr);
			hr = s_KeyboardDevice->SetCooperativeLevel(WinApp::g_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
			ENSURES(hr);
			hr = s_KeyboardDevice->Acquire();
			ENSURES(hr);
		}

		// マウス
		{
			hr = s_InputInterface->CreateDevice(GUID_SysMouse, &s_MouseDevice, nullptr);
			ENSURES(hr);
			hr = s_MouseDevice->SetDataFormat(&c_dfDIMouse);
			ENSURES(hr);
			hr = s_MouseDevice->SetCooperativeLevel(WinApp::g_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
			ENSURES(hr);
			hr = s_MouseDevice->Acquire();
			ENSURES(hr);
		}
	}

	void Update()
	{
		UpdateKeyboard();
		UpdateMouse();
	}

	bool IsKeyHold(Key keycode) noexcept
	{
		return IsKey(currentKey, keycode);
	}
	bool IsKeyDown(Key keycode) noexcept
	{
		return IsKey(currentKey, keycode) && !IsKey(prevKey, keycode);
	}
	bool IsKeyUp(Key keycode) noexcept
	{
		return !IsKey(currentKey, keycode) && IsKey(prevKey, keycode);
	}

	bool IsMouseDown(Mouse mouseCode) noexcept
	{
		return IsClick(currentMouse, mouseCode) && !IsClick(prevMouse, mouseCode);
	}
	bool IsMouseHold(Mouse mouseCode) noexcept
	{
		return IsClick(currentMouse, mouseCode);
	}
	bool IsMouseUp(Mouse mouseCode) noexcept
	{
		return !IsClick(currentMouse, mouseCode) && IsClick(prevMouse, mouseCode);
	}

	const Vector2& MousePosition() noexcept
	{
		return s_MousePosition;
	}
	const Vector2& MouseVelocity() noexcept
	{
		return s_MouseVelocity;
	}
	float MouseWheelVelocity() noexcept
	{
		return s_MouseVelocityZ;
	}
}
