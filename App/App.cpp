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
	TestModel model2;
};

CREATE_APPLICATION(App, 1280, 720);

void App::Startup(void)
{
	model.OnInit();
	model2.OnInit();
}

void App::Cleanup(void)
{
	model.OnTerm();
	model2.OnTerm();
}

void App::Update(float deltaT)
{
	deltaT++;

	model.Update();
	model2.Update();
}

void App::RenderScene(void)
{
	using namespace Graphics;
	auto cmdList = g_Command.Begin(Display::g_FrameIndex);

	// リソースバリアの設定
	auto barrier = GetTranslationBarrier(Display::g_RtvBuffer[Display::g_FrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	// リソースバリア
	cmdList->ResourceBarrier(1, &barrier);

	// レンダーターゲットの設定
	auto rtvHandle = Display::g_RtvBuffer[Display::g_FrameIndex].GetCpuHandle();
	auto dsvHandle = Display::g_DepthStencilBuffer[Display::g_FrameIndex].GetCpuHandle();
	cmdList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

	// クリアカラー
	float clearColor[] = { 0.0f,0.0f,1.0f,1.0f };

	// RTVをクリア
	cmdList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	// DSVをクリア
	cmdList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	model.Render(cmdList.Get());
	model2.Render(cmdList.Get());

	// リソースバリアの設定
	barrier = GetTranslationBarrier(Display::g_RtvBuffer[Display::g_FrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	// リソースバリア
	cmdList->ResourceBarrier(1, &barrier);

	g_Command.Finish();
}
