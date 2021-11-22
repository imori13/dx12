#include "GameCore.h"

class App : public GameCore::IGameApp
{
public:
	App(void) {}
	virtual void Startup(void) override;
	virtual void Cleanup(void) override;

	virtual void Update(float deltaT) override;
	virtual void RenderScene(void) override;
private:

};

CREATE_APPLICATION(App);

void App::Startup(void)
{

}

void App::Cleanup(void)
{
}

void App::Update(float deltaT)
{
}

void App::RenderScene(void)
{

}