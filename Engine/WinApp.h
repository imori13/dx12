#pragma once
#include "pch.h"

namespace Window
{
	// global
	extern uint32_t g_Width;
	extern uint32_t g_Height;
	extern HWND g_hWnd;

	void InitWnd(HINSTANCE hInstance, uint32_t width, uint32_t height);
	bool Update() noexcept;
	void TermWnd() noexcept;
}