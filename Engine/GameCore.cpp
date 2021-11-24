#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "GameCore.h"
#include "WinApp.h"
#include "GraphicsCore.h"
#include "Display.h"

namespace GameCore
{
	void InitializeApplication(IGameApp& game)
	{
		Graphics::Initialize();
		Display::Initialize();

		game.Startup();
	}

	bool UpdateApplication(IGameApp& game)
	{
		Graphics::ClearCommand();

		Display::Begin();

		game.Update(1.f / 60.f);
		game.RenderScene();

		Display::End();

		auto nextFrame = Graphics::ExecuteCommandList(Graphics::g_pCmdList);
		Graphics::WaitForFence(nextFrame);

		Display::Present(1);

		return !game.IsDone();
	}

	void TerminateApplication(IGameApp& game)
	{
		Graphics::IdleGpu();

		game.Cleanup();
		Display::Terminate();
		Graphics::Terminate();
	}

	bool IGameApp::IsDone(void)
	{
		return false;
	}

	int RunApplication(IGameApp& game, HINSTANCE hInstance, int nCmdShow, uint32_t width, uint32_t height)
	{
		// ���������[�N�`�F�b�N
#if defined(DEBUG) || defined(_DEBUG)
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		//_CrtSetBreakAlloc(n);
#endif

		// �E�B���h�E������
		auto hr = Window::WinApp::InitWnd(hInstance, nCmdShow, width, height);
		if(FAILED(hr))
		{ return false; }

		// �A�v��������
		InitializeApplication(game);

		do
		{
			// �E�B���h�E�X�V
			auto flag = Window::WinApp::Update();

			// �E�B���h�E��false�ŏI��
			if(flag) { break; }

			// �A�v���̍X�V
		} while(UpdateApplication(game));

		// �A�v���I��
		TerminateApplication(game);
		// �E�B���h�E�I��
		Window::WinApp::TermWnd();

		return 0;
	}
}