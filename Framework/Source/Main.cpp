#include "WindowApp.h"

_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
	WindowApp winapp = WindowApp(hInstance, nCmdShow, 1280, 720);

	return 0;
}