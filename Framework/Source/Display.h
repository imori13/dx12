#pragma once

namespace Display
{
	void Initialize(void);
	void Shutdown(void);
	void Resize(uint32_t width, uint32_t height);
	void Present(void);
}

namespace Graphics
{
	extern uint32_t g_DisplayWidth;
	extern uint32_t g_DisplayHeight;

	// アプリケーションの開始から経過したフレームの数
	uint64_t GetFrameCount(void);
	// フレーム時間
	float GetFrameTime(void);
	// 1秒あたりの合計フレーム数
	float GetFrameRate(void);
}
