#pragma once
#include "pch.h"

namespace Window
{
	// global
	extern uint32_t g_Width;
	extern uint32_t g_Height;
	extern HWND g_hWnd;

	class WinApp
	{
	public:
		static bool InitWnd(HINSTANCE hInstance, int nCmdShow, uint32_t width, uint32_t height);
		static bool Update();
		static void TermWnd();

	private:
		static HINSTANCE m_hInst;
		static const wchar_t* m_windowName;
	};
}