#pragma once

namespace GameCore
{
	class IGameApp
	{
	public:
		virtual void Startup(void) = 0;
		virtual void Cleanup(void) = 0;
		virtual bool IsDone(void);
		virtual void Update(float deltaT) = 0;
		virtual void RenderScene(void) = 0;
	};
}

namespace GameCore
{
	int RunApplication(IGameApp& app, HINSTANCE hInstance, int nCmdShow, uint32_t width, uint32_t height);
}

#define CREATE_APPLICATION( app_class , width, height) \
    int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE , _In_ LPWSTR , _In_ int nCmdShow) \
    { \
		app_class app = app_class();\
        return GameCore::RunApplication(app, hInstance, nCmdShow, width, height ); \
    }