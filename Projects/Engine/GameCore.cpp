#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "GameCore.h"
#include "WinApp.h"
#include "GraphicsCore.h"
#include "Display.h"
#include "Command.h"
#include "TimeStamp.h"

#include <boost/version.hpp>

namespace GameCore
{
	float g_DrawSumT;
	float g_WriteT;
	float g_FrameWaitT;
	float g_GpuWaitT;

	void InitializeApplication(IGameApp& game)
	{
		TimeStamp::Initialize();

		TimeStamp::Begin(L"����������");

		Graphics::Initialize();
		Command::Initialize();
		Display::Initialize();
		game.Startup();

		TimeStamp::End(L"����������");
	}

	bool UpdateApplication(IGameApp& game)
	{
		TimeStamp::Begin(L"�`�揈��");

		// �X�V����
		{
			TimeStamp::Begin(L"�`�揑������");

			game.Update(0);
		}

		// �`�揈��
		{
			game.RenderScene();

			g_WriteT = TimeStamp::End(L"�`�揑������");
		}

		// ��ʕ\��
		{
			TimeStamp::Begin(L"��ʕ\��");

			Display::Present(1);

			g_FrameWaitT = TimeStamp::End(L"��ʕ\��");
		}

		// GPU�ҋ@
		{
			TimeStamp::Begin(L"GPU�ҋ@");

			Command::MoveToNextFrame();

			g_GpuWaitT = TimeStamp::End(L"GPU�ҋ@");
		}

		g_DrawSumT = TimeStamp::End(L"�`�揈��");

		return !game.IsDone();
	}

	void TerminateApplication(IGameApp& game)
	{
		Command::WaitForGpu();

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
		//_CrtSetBreakAlloc(266);
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