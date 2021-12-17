#include "GameCore.h"
#include "Display.h"
#include "TranslationBarrirUtil.h"
#include "TestModel.h"
#include "WinApp.h"
#include "Command.h"
#include "App_ImGui.h"
#include "Timer.h"
#include "ResourceManager.h"
#include "PipelineInitializer.h"
#include "ObjLoader.h"
#include "Renderer.h"

class App : public GameCore::IGameApp
{
public:
	App() noexcept {};

	void Startup(void) override;
	void Cleanup(void) override;
	void Update(float deltaT) override;
	void RenderScene(void) override;
private:
	//TestModel model;
	//TestModel model2;
};

CREATE_APPLICATION(App, 1600, 900);

void App::Startup(void)
{
	App_ImGui::Initialize();

	std::wstring path = L"Textures/";
	//ResourceManager::LoadTexture(path + L"pixel.png");
	ResourceManager::LoadTexture(path + L"neko.jpg");
	//ResourceManager::LoadTexture(path + L"neko2.jpg");
	ResourceManager::LoadTexture(path + L"umaru.jpg");
	//ResourceManager::LoadTexture(path + L"spot_texture.png");
	//ResourceManager::LoadTexture(path + L"gf_g36_dif_04.png");

	path = L"Models/";
	ResourceManager::LoadMesh(path + L"umaru.obj");
	ResourceManager::LoadMesh(path + L"Cube.obj");
	//ResourceManager::LoadMesh(path + L"Monkey.obj");
	//ResourceManager::LoadMesh(path + L"spot_triangulated.obj");
	//ResourceManager::LoadMesh(path + L"g36.obj");

	ResourceManager::LoadShader(L"iMoriDefaultVS.cso");
	ResourceManager::LoadShader(L"iMoriDefaultPS.cso");

	PipelineInitializer::Initialize(L"iMoriDefaultVS.cso", L"iMoriDefaultPS.cso");

	Renderer::Load(L"umaru", L"umaru.obj", L"umaru.jpg");

	//model.OnInit(L"umaru.obj");
	//model.SetTexture(L"umaru.jpg");
	//model2.OnInit(L"g36.obj");
	//model2.SetTexture(L"gf_g36_dif_04.png");

	//model.m_Position = { 1.0f,-0.9f ,0 };
	//model2.m_Position = { -1.0f,-0.7f ,0 };

	//constexpr float scale = 0.02f;
	//model.m_Scale = { scale,scale ,scale };
	//scale = 8.0f;
	//model2.m_Scale = { scale,scale ,scale };
}

void App::Cleanup(void)
{
	App_ImGui::Terminate();

	//model.OnTerm();
	//model2.OnTerm();
}

void App::Update(float deltaT)
{
	App_ImGui::Update();

	deltaT++;

	//model.m_Rotate.y += 1.f * Timer::g_FrameTime;
	//model2.m_Rotate.y += 1.f * Timer::g_FrameTime;

	//model.Update();
	//model2.Update();
}

void App::RenderScene(void)
{
	using namespace Graphics;
	auto cmdList = Command::Begin(Display::g_FrameIndex);

	EXPECTS(cmdList != nullptr);

	{
		// リソースバリア
		auto barrier = GetTranslationBarrier(Display::g_RenderTargetBuffer.at(Display::g_FrameIndex).Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		cmdList->ResourceBarrier(1, &barrier);
	}

	// レンダーターゲットの設定
	const auto rtvHandle = Display::g_RenderTargetBuffer.at(Display::g_FrameIndex).GetCpuHandle();
	const auto dsvHandle = Display::g_DepthStencilBuffer.at(Display::g_FrameIndex).GetCpuHandle();
	cmdList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

	// クリアカラー
	const float clearColor[4] = { 0.05f,0.05f,0.05f,1.0f };

	// RTVをクリア
	cmdList->ClearRenderTargetView(rtvHandle, gsl::make_span(clearColor).data(), 0, nullptr);
	// DSVをクリア
	cmdList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	cmdList->RSSetViewports(1, &Display::g_Viewport);
	cmdList->RSSetScissorRects(1, &Display::g_Scissor);

	//model.Render(cmdList.Get());
	//model2.Render(cmdList.Get());

	Renderer::Draw(cmdList.Get(), L"umaru");

	App_ImGui::Render(cmdList.Get());

	{
		// リソースバリア
		auto barrier = GetTranslationBarrier(Display::g_RenderTargetBuffer.at(Display::g_FrameIndex).Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		cmdList->ResourceBarrier(1, &barrier);
	}


	Command::End();

	App_ImGui::UpdateAdditionalPlatformWindows(cmdList.Get());
}
