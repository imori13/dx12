#include "pch.h"
#include "ResourceHeap.h"
#include "GraphicsCore.h"

void ResourceHeap::Create(uint32_t descriptorCount, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flag)
{
	m_DescriptorCount = descriptorCount;

	// �q�[�v�̐ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = descriptorCount;
	heapDesc.Type = type;
	heapDesc.Flags = flag;
	heapDesc.NodeMask = 0;

	// �q�[�v����
	const auto hr = Graphics::g_pDevice->CreateDescriptorHeap(
		&heapDesc,
		IID_PPV_ARGS(m_pHeap.GetAddressOf()));
	ENSURES(hr, "PixelHeap����");

	// �n���h���ݒ�
	m_CpuHandle = m_pHeap->GetCPUDescriptorHandleForHeapStart();

	// �C���N�������g�T�C�Y�擾
	m_IncrementSize = Graphics::g_pDevice->GetDescriptorHandleIncrementSize(type);
}
