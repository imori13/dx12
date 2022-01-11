#pragma once

namespace GameCore
{
	class IGameApp
	{
	public:
		IGameApp() = default;
		virtual ~IGameApp() = default;
		IGameApp(const IGameApp&) = default;
		IGameApp(IGameApp&&) = default;
		IGameApp& operator=(const IGameApp&) = default;
		IGameApp& operator=(IGameApp&&) = default;

		virtual void Startup(void) = 0;
		virtual void Cleanup(void) = 0;
		virtual bool IsDone(void) noexcept;
		virtual void Update() = 0;
		virtual void UpdateGUI() = 0;
		virtual void RenderScene(void) = 0;
		virtual void RenderGUI(void) = 0;
	};
}

namespace GameCore
{
	extern bool g_IsSync;

	int RunApplication(IGameApp& app, HINSTANCE hInstance, uint32_t width, uint32_t height);
}


#define CREATE_APPLICATION( app_class , width, height) \
    int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE , _In_ LPWSTR , _In_ int) \
    { \
		app_class app = app_class();\
        return GameCore::RunApplication(app, hInstance, width, height ); \
    }