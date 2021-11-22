#include "GameCore.h"
#include "WinApp.h"

namespace GameCore
{
	void InitializeApplication()
	{

	}

	bool UpdateApplication()
	{
		return true;
	}

	void TerminateApplication()
	{

	}

	int RunApplication(HINSTANCE hInstance, int nCmdShow, uint32_t width, uint32_t height)
	{
		auto hr = MyWindow::Create(hInstance, nCmdShow, width, height);
		if(FAILED(hr))
		{ return false; }

		InitializeApplication();

		do
		{
			auto flag = MyWindow::Update();

			// ウィンドウがfalseで終了
			if(flag) { break; }
		} while(UpdateApplication());	// アプリがfalseで終了

		TerminateApplication();

		return 0;
	}
}