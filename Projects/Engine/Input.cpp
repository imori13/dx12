#include "Input.h"
#include "WinApp.h"
#include "Debug.h"

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
	constexpr inline void DebugHRESULT(HRESULT hr) noexcept
	{
		// 問題なし(停止しない) : ウィンドウが裏面(デバイス状態は取得されない)
		if(FAILED(hr))
			LOGLINE("WARNING : Input.cpp DebugHRESULT(hr) 画面未検出");
	}

	void UpdateKeyboard() noexcept
	{
		// キーボード状態取得
		{
			prevKey = currentKey;

			auto hr = s_KeyboardDevice->GetDeviceState(gsl::narrow<DWORD>(currentKey.size() * sizeof(BYTE)), currentKey.data());
			if(FAILED(hr))
			{
				hr = s_KeyboardDevice->Acquire();
				DebugHRESULT(hr);
			}
		}
	}
	void UpdateMouse()
	{
		// マウス状態取得
		{
			prevMouse = currentMouse;
			auto hr = s_MouseDevice->GetDeviceState(sizeof(DIMOUSESTATE), &currentMouse);

			if(FAILED(hr))
			{
				hr = s_MouseDevice->Acquire();
				DebugHRESULT(hr);
			}
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

	constexpr inline uint32_t MOUSE_ON_VALUE = 0x80;
	constexpr inline bool IsClick(const DIMOUSESTATE& state, Mouse mousecode)
	{
		return gsl::at(state.rgbButtons, static_cast<uint32_t>(mousecode)) & MOUSE_ON_VALUE;
	}

	constexpr inline bool IsKey(gsl::span<BYTE> state, Keys keycode)
	{
		return state[static_cast<uint32_t>(keycode)];
	}
}

namespace Input
{
	void Initialize() noexcept
	{
		auto hr = DirectInput8Create(
			WinApp::g_hInst, DIRECTINPUT_VERSION, IID_IDirectInput8,
			reinterpret_cast<LPVOID*>(&s_InputInterface), nullptr);
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
			DebugHRESULT(hr);
			hr = s_KeyboardDevice->Poll();
			DebugHRESULT(hr);
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
			DebugHRESULT(hr);
			hr = s_MouseDevice->Poll();
			DebugHRESULT(hr);
		}
	}

	void Update()
	{
		UpdateKeyboard();
		UpdateMouse();
	}

	void Terminate() noexcept
	{
		HRESULT hr = {};
		uint64_t counter = 0;

		// キーボードの破棄
		{
			hr = s_KeyboardDevice->Unacquire();
			ENSURES(hr);

			counter = s_KeyboardDevice->Release();
			ENSURES(counter == 0);
		}

		// マウスの破棄
		{
			hr = s_MouseDevice->Unacquire();
			ENSURES(hr);

			counter = s_MouseDevice->Release();
			ENSURES(counter == 0);
		}

		// インターフェースの破棄
		counter = s_InputInterface->Release();
		ENSURES(counter == 0);

		LOGLINE("InputDevice破棄");
	}

	bool IsKeyHold(Keys keycode) noexcept
	{
		return IsKey(currentKey, keycode);
	}
	bool IsKeyDown(Keys keycode) noexcept
	{
		return IsKey(currentKey, keycode) && !IsKey(prevKey, keycode);
	}
	bool IsKeyUp(Keys keycode) noexcept
	{
		return !IsKey(currentKey, keycode) && IsKey(prevKey, keycode);
	}

	bool IsMouseDown(Mouse mousecode) noexcept
	{
		return IsClick(currentMouse, mousecode) && !IsClick(prevMouse, mousecode);
	}
	bool IsMouseHold(Mouse mousecode) noexcept
	{
		return IsClick(currentMouse, mousecode);
	}
	bool IsMouseUp(Mouse mousecode) noexcept
	{
		return !IsClick(currentMouse, mousecode) && IsClick(prevMouse, mousecode);
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
