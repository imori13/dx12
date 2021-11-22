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

			// ウィンドウがfalseで終了
			if(flag) { break; }
		} while(UpdateApplication());	// アプリがfalseで終了

		TerminateApplication();

		return 0;
	}
}