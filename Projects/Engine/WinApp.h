#pragma once

namespace WinApp
{
	// global
	extern uint32_t g_Width;
	extern uint32_t g_Height;
	extern HWND g_hWnd;

	void InitWnd(HINSTANCE hInstance, uint32_t width, uint32_t height);
	bool Update() noexcept;
	void TermWnd() noexcept;

	extern LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
}