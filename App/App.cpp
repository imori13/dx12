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

	// ���\�[�X�o���A�̐ݒ�
	auto barrier = GetTranslationBarrier(Display::g_RtvBuffer[Display::g_FrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	// ���\�[�X�o���A
	cmdList->ResourceBarrier(1, &barrier);

	// �����_�[�^�[�Q�b�g�̐ݒ�
	auto rtvHandle = Display::g_RtvBuffer[Display::g_FrameIndex].GetCpuHandle();
	auto dsvHandle = Display::g_DepthStencilBuffer[Display::g_FrameIndex].GetCpuHandle();
	cmdList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

	// �N���A�J���[
	float clearColor[] = { 0.0f,0.0f,1.0f,1.0f };

	// RTV���N���A
	cmdList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	// DSV���N���A
	cmdList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	model.Render(cmdList.Get());
	model2.Render(cmdList.Get());

	// ���\�[�X�o���A�̐ݒ�
	barrier = GetTranslationBarrier(Display::g_RtvBuffer[Display::g_FrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	// ���\�[�X�o���A
	cmdList->ResourceBarrier(1, &barrier);

	g_Command.Finish();
}
