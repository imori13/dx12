#include "GameCore.h"
#include "WinApp.h"
#include "GraphicsCore.h"
#include "Display.h"

namespace GameCore
{
	void InitializeApplication()
	{
		Graphics::Initialize();
		Display::Initialize();
	}

	bool UpdateApplication()
	{
		Graphics::ClearCommand();
		Display::Begin();
		Display::End();

		return true;
	}

	void TerminateApplication()
	{
		Display::Terminate();
		Graphics::Terminate();
	}

	int RunApplication(HINSTANCE hInstance, int nCmdShow, uint32_t width, uint32_t height)
	{
		// �E�B���h�E������
		auto hr = Window::WinApp::InitWnd(hInstance, nCmdShow, width, height);
		if(FAILED(hr))
		{ return false; }

		// �A�v��������
		InitializeApplication();

		do
		{
			// �E�B���h�E�X�V
			auto flag = Window::WinApp::Update();

			// �E�B���h�E��false�ŏI��
			if(flag) { break; }

			// �A�v���̍X�V
		} while(UpdateApplication());

		// �A�v���I��
		TerminateApplication();
		// �E�B���h�E�I��
		Window::WinApp::TermWnd();

		return 0;
	}
}