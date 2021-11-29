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
		game.Update(0);
		game.RenderScene();

		Display::Present(1);

		return !game.IsDone();
	}

	void TerminateApplication(IGameApp& game)
	{
		Graphics::g_Command.WaitForGpu();

		game.Cleanup();
		Display::Terminate();
		Graphics::Terminate();
	}

	bool IGameApp::IsDone(void) noexcept
	{
		return false;
	}

	int RunApplication(IGameApp& game, HINSTANCE hInstance, uint32_t width, uint32_t height)
	{
		// ���������[�N�`�F�b�N
#if defined(DEBUG) || defined(_DEBUG)
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		//_CrtSetBreakAlloc(n);
#endif
		// �E�B���h�E������
		Window::InitWnd(hInstance, width, height);

		// �A�v��������
		InitializeApplication(game);

		LOGLINE("������ ���������� ������");

		do
		{
			// �E�B���h�E�X�V
			const auto flag = Window::Update();

			// �E�B���h�E��false�ŏI��
			if(flag) { break; }

			// �A�v���̍X�V
		} while(UpdateApplication(game));

		// �A�v���I��
		TerminateApplication(game);
		// �E�B���h�E�I��
		Window::TermWnd();

		LOGLINE("������ �I������ ������");

		return 0;
	}
}