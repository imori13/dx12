#pragma once

namespace GameCore
{
	void InitializeApplication();
	bool UpdateApplication();
	void TerminateApplication();

	int RunApplication(HINSTANCE hInstance, int nCmdShow, uint32_t width, uint32_t height);
};