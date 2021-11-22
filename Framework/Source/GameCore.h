#pragma once

#include <stdint.h>
#include <windows.h>
#include "D3DApp.h"

class GameCore
{
public:
	// public•Ï”

	// publicŠÖ”
	static int Run(D3DApp* app, HINSTANCE hInst, int nCmdShow);
	static HWND GetHWND() { return m_hWnd; }
private:
	// private•Ï”
	static HWND m_hWnd;

	// privateŠÖ”
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

