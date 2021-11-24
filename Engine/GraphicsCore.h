#pragma once
#include "Display.h"

using namespace Microsoft::WRL;

namespace Graphics
{
	extern ComPtr<ID3D12Device> g_pDevice;					// �f�o�C�X
	extern ComPtr<ID3D12CommandAllocator> g_pCmdAllocator[FRAME_COUNT];
	extern ComPtr<ID3D12GraphicsCommandList> g_pCmdList;	// �R�}���h���X�g

	bool Initialize();
	void Terminate();
}