#include "GameCore.h"
#include "Display.h"
#include "Command.h"
#include "App_ImGui.h"
#include "ResourceManager.h"
#include "PipelineInitializer.h"
#include "Renderer.h"
#include "Vector3.h"
#include "Timer.h"

class App : public GameCore::IGameApp
{
public:
	App() noexcept {};

	void Startup(void) override;
	void Cleanup(void) override;
	void Update(float deltaT) override;
	void RenderScene(void) override;

	Camera camera;
};

CREATE_APPLICATION(App, 1600, 900);

void App::Startup(void)
{
	App_ImGui::Initialize();

	std::wstring path = L"Textures/";
	ResourceManager::LoadTexture(path + L"pixel.png");
	ResourceManager::LoadTexture(path + L"neko.jpg");
	ResourceManager::LoadTexture(path + L"neko2.jpg");
	ResourceManager::LoadTexture(path + L"umaru.jpg");

	path = L"Models/";
	ResourceManager::LoadMesh(path + L"umaru.obj");

	ResourceManager::LoadShader(L"iMoriDefaultVS.cso");
	ResourceManager::LoadShader(L"iMoriDefaultPS.cso");

	PipelineInitializer::Initialize(L"iMoriDefaultVS.cso", L"iMoriDefaultPS.cso");

	Renderer::Load(L"umaru", L"umaru.obj", L"umaru.jpg");

	camera.Create(90, 0.01f, 1000.0f);
	camera.Position = Vector3(0, 0, -5);
}

void App::Cleanup(void)
{
	App_ImGui::Terminate();
}

void App::Update(float deltaT)
{
	App_ImGui::Update();

	camera.Update();

	deltaT++;
}

void App::RenderScene(void)
{
	auto cmdList = Renderer::Begin();

	constexpr Vector3 position(0, -1, 0);
	constexpr Vector3 scale(0.02f);
	const Vector3 rotation(0, static_cast<float>(Timer::g_ElapsedTime) * 0.1f, 0);

	auto world = Matrix4x4::Identity();
	world *= Matrix4x4::Scale(scale);
	world *= Matrix4x4::RotateRollPitchYaw(rotation);
	world *= Matrix4x4::Translate(position);

	Renderer::Draw(L"umaru", world, camera);
	Renderer::SendCommand(cmdList);

	App_ImGui::Render(cmdList);
	Renderer::End(cmdList);
	App_ImGui::UpdateAdditionalPlatformWindows(cmdList);
}
