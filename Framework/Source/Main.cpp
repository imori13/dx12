#include "GameCore.h"

_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
	D3DApp app(1280, 720, L"ƒ^ƒCƒgƒ‹");
	return GameCore::Run(&app, hInstance, nCmdShow);
}