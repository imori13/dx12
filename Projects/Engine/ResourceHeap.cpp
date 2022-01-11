#include "pch.h"
#include "ResourceHeap.h"
#include "GraphicsCore.h"
#include "Debug.h"

void ResourceHeap::Create(uint32_t descriptorCount, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flag)
{
	m_DescriptorCount = descriptorCount;

	// ヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = descriptorCount;
	heapDesc.Type = type;
	heapDesc.Flags = flag;
	heapDesc.NodeMask = 0;

	// ヒープ生成
	const auto hr = Graphics::g_pDevice->CreateDescriptorHeap(
		&heapDesc,
		IID_PPV_ARGS(m_pHeap.GetAddressOf()));
	ENSURES(hr, "PixelHeap生成");

	// ハンドル設定
	m_Handle.CPU = m_pHeap->GetCPUDescriptorHandleForHeapStart();
	if(flag == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
	{ m_Handle.GPU = m_pHeap->GetGPUDescriptorHandleForHeapStart(); }

	// インクリメントサイズ取得
	m_IncrementSize = Graphics::g_pDevice->GetDescriptorHandleIncrementSize(type);
}