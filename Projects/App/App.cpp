#include "GameCore.h"
#include "Display.h"
#include "Command.h"
#include "App_ImGui.h"
#include "ResourceManager.h"
#include "PipelineInitializer.h"
#include "Renderer.h"
#include "Vector3.h"
#include "Vector2.h"
#include "Timer.h"
#include "Input.h"

class App : public GameCore::IGameApp
{
public:
	App() noexcept {};

	void Startup(void) override;
	void Cleanup(void) override;
	void Update(float deltaT) override;
	void RenderScene(void) override;

	Camera camera;
	Matrix4x4 world;
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
	camera.Foward = Matrix4x4::LookAt(camera.Position, Vector3::Zero(), Vector3::Up()) * Vector3::One();
}

void App::Cleanup(void)
{
	App_ImGui::Terminate();
}

void App::Update(float deltaT)
{
	deltaT++;	// åxçêâÒîóp

	App_ImGui::Update();

	camera.Update();

	static Vector3 position(0, -1, 0);
	static Vector3 scale(0.02f);
	static Vector3 rotation(0, 1, 0);

	world = Matrix4x4::Identity();
	world *= Matrix4x4::Scale(scale);
	world *= Matrix4x4::RotateAxis(rotation, Timer::g_ElapsedTime * 1.0f);
	world *= Matrix4x4::Translate(position);

	Renderer::Draw(L"umaru", world, camera);

	App_ImGui::Begin("Parameter");
	App_ImGui::DragSlider("camera pos", &camera.Position);
	App_ImGui::DragSlider("camera rot", &camera.Rotation);
	App_ImGui::DragSlider("obj pos", &position);
	App_ImGui::DragSlider("obj rot", &rotation);
	App_ImGui::DragSlider("obj sca", &scale);
	App_ImGui::End();

	rotation.Normalize();
}

void App::RenderScene(void)
{
	auto cmdList = Renderer::Begin();

	Renderer::SendCommand(cmdList);
	App_ImGui::Render(cmdList);
	Renderer::End(cmdList);

	App_ImGui::UpdateAdditionalPlatformWindows(cmdList);
}
