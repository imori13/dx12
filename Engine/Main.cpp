#include "GameCore.h"

_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
	return GameCore::RunApplication(hInstance, nCmdShow, 1280, 720);
}