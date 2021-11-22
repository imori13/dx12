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

	// �A�v���P�[�V�����̊J�n����o�߂����t���[���̐�
	uint64_t GetFrameCount(void);
	// �t���[������
	float GetFrameTime(void);
	// 1�b������̍��v�t���[����
	float GetFrameRate(void);
}
