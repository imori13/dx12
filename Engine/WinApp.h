#pragma once
#include "pch.h"

namespace Window
{
	// global
	extern uint32_t g_Width;
	extern uint32_t g_Height;
	extern HINSTANCE g_hInst;
	extern HWND g_hWnd;
	extern const wchar_t* g_windowName;

	bool InitWnd(HINSTANCE hInstance, int nCmdShow, uint32_t width, uint32_t height);
	bool Update();
	void TermWnd();
}