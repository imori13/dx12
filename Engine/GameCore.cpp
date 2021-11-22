#include "GameCore.h"
#include "WinApp.h"
#include "HogeApp.h"

namespace GameCore
{
	HogeApp app = HogeApp();

	void InitializeApplication()
	{
		app.InitApp();
	}

	bool UpdateApplication()
	{
		app.Render();

		return true;
	}

	void TerminateApplication()
	{
		app.TermApp();
		Window::TermWnd();
	}

	int RunApplication(HINSTANCE hInstance, int nCmdShow, uint32_t width, uint32_t height)
	{
		auto hr = Window::InitWnd(hInstance, nCmdShow, width, height);
		if(FAILED(hr))
		{ return false; }

		InitializeApplication();

		do
		{
			auto flag = Window::Update();

			// �E�B���h�E��false�ŏI��
			if(flag) { break; }
		} while(UpdateApplication());	// �A�v����false�ŏI��

		TerminateApplication();

		return 0;
	}
}