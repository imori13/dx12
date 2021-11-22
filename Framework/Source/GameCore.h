#pragma once

#include <stdint.h>
#include <windows.h>
#include "D3DApp.h"

class GameCore
{
public:
	// public変数

	// public関数
	static int Run(D3DApp* app, HINSTANCE hInst, int nCmdShow);
	static HWND GetHWND() { return m_hWnd; }
private:
	// private変数
	static HWND m_hWnd;

	// private関数
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

