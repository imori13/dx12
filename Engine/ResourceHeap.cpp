#include "pch.h"
#include "ResourceHeap.h"
#include "GraphicsCore.h"

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
	m_HeapHandle_CPU = m_pHeap->GetCPUDescriptorHandleForHeapStart();
	if(flag == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
	{ m_HeapHandle_GPU = m_pHeap->GetGPUDescriptorHandleForHeapStart(); }

	// インクリメントサイズ取得
	m_IncrementSize = Graphics::g_pDevice->GetDescriptorHandleIncrementSize(type);
}

D3D12_CPU_DESCRIPTOR_HANDLE ResourceHeap::GetCPUHandle(uint32_t index) noexcept
{
	EXPECTS(index >= 0 && index < m_DescriptorCount);

	auto copyHandle = m_HeapHandle_CPU;
	copyHandle.ptr += static_cast<uint64_t>(index) * static_cast<uint64_t>(m_IncrementSize);
	return copyHandle;
}

D3D12_GPU_DESCRIPTOR_HANDLE ResourceHeap::GetGPUHandle(uint32_t index) noexcept
{
	EXPECTS(index >= 0 && index < m_DescriptorCount);

	auto copyHandle = m_HeapHandle_GPU;
	copyHandle.ptr += static_cast<uint64_t>(index) * static_cast<uint64_t>(m_IncrementSize);
	return copyHandle;
}
