#include "pch.h"
#include "GameCore.h"
#include "GraphicsCore.h"
#include "Display.h"
#include "TranslationBarrirUtil.h"
#include "TestModel.h"

class App : public GameCore::IGameApp
{
public:
	App() {}
	~App() {}

	virtual void Startup(void) override;
	virtual void Cleanup(void) override;
	virtual void Update(float deltaT) override;
	virtual void RenderScene(void) override;
private:
	TestModel model;
};

CREATE_APPLICATION(App, 1280, 720);

void App::Startup(void)
{
	model.OnInit();
}

void App::Cleanup(void)
{
	model.OnTerm();
}

void App::Update(float deltaT)
{
	deltaT++;

	model.Update();
}

void App::RenderScene(void)
{
	using namespace Graphics;
	auto cmdList = g_Command.Begin(Display::g_FrameIndex);

	// リソースバリアの設定
	auto barrier = GetTranslationBarrier(Display::g_RtvBuffer[Display::g_FrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	//gfxContext.TransitionResource(g_SceneColorBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, true);
	//gfxContext.ClearColor(g_SceneColorBuffer);

	// リソースバリア
	cmdList->ResourceBarrier(1, &barrier);

	// レンダーターゲットの設定
	cmdList->OMSetRenderTargets(1, &Display::g_RtvBuffer[Display::g_FrameIndex].m_CpuHandle, FALSE, nullptr);

	// クリアカラー
	float clearColor[] = { 0.0f,0.0f,1.0f,1.0f };

	// レンダーターゲットビューをクリア
	cmdList->ClearRenderTargetView(Display::g_RtvBuffer[Display::g_FrameIndex].m_CpuHandle, clearColor, 0, nullptr);

	model.Render(cmdList.Get());

	// リソースバリアの設定
	barrier = GetTranslationBarrier(Display::g_RtvBuffer[Display::g_FrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	// リソースバリア
	cmdList->ResourceBarrier(1, &barrier);

	g_Command.Finish();
}
