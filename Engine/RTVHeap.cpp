#include "RTVHeap.h"
#include "GraphicsCore.h"

void RTVHeap::Create(uint32_t DescriptorSize)
{
	m_DescriptorSize = DescriptorSize;

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = DescriptorSize;	// ダブルバッファ用で2個
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	desc.NodeMask = 0;

	auto hr = Graphics::g_pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_pHeap.GetAddressOf()));
	if(FAILED(hr))
	{ return; }

	m_IncrementSize = Graphics::g_pDevice->GetDescriptorHandleIncrementSize(desc.Type);
	m_CpuHandle = m_pHeap->GetCPUDescriptorHandleForHeapStart();
}