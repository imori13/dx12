#pragma once
//
//#include "PipelineState.h"
//#include "DescriptorHeap.h"
//#include "RootSignature.h"
//#include "GraphicsCommon.h"

#include "pch.h"

namespace Graphics
{
	using namespace Microsoft::WRL;

	void Initialize(void);
	void Shutdown(void);

	static const uint32_t FrameCount = 2;								// �t���[���o�b�t�@��
	extern ComPtr<ID3D12Device> m_pDevice;								// �f�o�C�X
	extern ComPtr<ID3D12CommandQueue> m_pQueue;							// �R�}���h�L���[
	extern ComPtr<IDXGISwapChain3> m_pSwapChain;						// �X���b�v�`�F�C��
	extern ComPtr<ID3D12Resource> m_pColorBuffer[FrameCount];			// �J���[�o�b�t�@
	extern ComPtr<ID3D12CommandAllocator> m_pCmdAllocator[FrameCount];	// �R�}���h�A���P�[�^
	extern ComPtr<ID3D12GraphicsCommandList> m_pCmdList;				// �R�}���h���X�g
	extern ComPtr<ID3D12DescriptorHeap> m_pHeapRTV;						// RTV�q�[�v
	extern ComPtr<ID3D12Fence> m_pFence;								// �t�F���X
}