#pragma once
#include "WinApp.h"

using namespace Microsoft::WRL;

namespace Display
{
#define FRAME_COUNT 2

	// ������
	bool Initialize(void);
	// �I��
	void Terminate(void);
	// �`���t
	void Begin(ComPtr<ID3D12GraphicsCommandList> cmdList);
	// �`��I��
	void End(ComPtr<ID3D12GraphicsCommandList> cmdList);
	// �\������
	void Present(uint32_t interval);
}