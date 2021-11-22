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

			// �E�B���h�E��false�ŏI��
			if(flag) { break; }
		} while(UpdateApplication());	// �A�v����false�ŏI��

		TerminateApplication();

		return 0;
	}
}