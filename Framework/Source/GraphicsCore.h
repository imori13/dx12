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

	static const uint32_t FrameCount = 2;								// フレームバッファ数
	extern ComPtr<ID3D12Device> m_pDevice;								// デバイス
	extern ComPtr<ID3D12CommandQueue> m_pQueue;							// コマンドキュー
	extern ComPtr<IDXGISwapChain3> m_pSwapChain;						// スワップチェイン
	extern ComPtr<ID3D12Resource> m_pColorBuffer[FrameCount];			// カラーバッファ
	extern ComPtr<ID3D12CommandAllocator> m_pCmdAllocator[FrameCount];	// コマンドアロケータ
	extern ComPtr<ID3D12GraphicsCommandList> m_pCmdList;				// コマンドリスト
	extern ComPtr<ID3D12DescriptorHeap> m_pHeapRTV;						// RTVヒープ
	extern ComPtr<ID3D12Fence> m_pFence;								// フェンス
}