#include "pch.h"
#include "PixelHeap.h"
#include "GraphicsCore.h"

void PixelHeap::Create(uint32_t descriptorCount, D3D12_DESCRIPTOR_HEAP_TYPE flag)
{
	m_DescriptorCount = descriptorCount;

	// ヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = descriptorCount;
	heapDesc.Type = flag;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	heapDesc.NodeMask = 0;

	// ヒープ生成
	const auto hr = Graphics::g_pDevice->CreateDescriptorHeap(
		&heapDesc,
		IID_PPV_ARGS(m_pHeap.GetAddressOf()));
	if(FAILED(hr))
	{ return; }

	// ハンドル設定
	m_CpuHandle = m_pHeap->GetCPUDescriptorHandleForHeapStart();

	// インクリメントサイズ取得
	m_IncrementSize = Graphics::g_pDevice->GetDescriptorHandleIncrementSize(flag);
}
