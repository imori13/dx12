#include <iostream>
#include <imgui.h>
#include "Window.h"

using namespace UIEditor;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Window window{};
	Window subWindow{};

	window.Create(L"BaseWindow", nullptr, 1280, 720);
	subWindow.Create(L"ChildWindow", window.m_hWnd, 600, 200, true);

	do
	{
		const bool flag = window.Update();
		subWindow.Update();

		if(flag) { break; }

	} while(true);

	window.Term();
	subWindow.Term();

	return 0;
}