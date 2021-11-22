#pragma once

#include <stdint.h>
#include <windows.h>
#include "D3DApp.h"

class GameCore
{
public:
	// public�ϐ�

	// public�֐�
	static int Run(D3DApp* app, HINSTANCE hInst, int nCmdShow);
	static HWND GetHWND() { return m_hWnd; }
private:
	// private�ϐ�
	static HWND m_hWnd;

	// private�֐�
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

