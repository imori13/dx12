#pragma once

class DescriptorHeap
{
public:
	DescriptorHeap() noexcept
		: m_CpuHandle{}
		, m_DescriptorCount(0)
		, m_IncrementSize(0)
	{}

	// ����
	void Create(uint32_t bufferCount, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flag);
	// �w��̃n���h���擾
	D3D12_CPU_DESCRIPTOR_HANDLE GetHandle(uint32_t index) noexcept
	{
		EXPECTS(index >= 0 && index < m_DescriptorCount);

		auto copyHandle = m_CpuHandle;
		copyHandle.ptr += static_cast<uint64_t>(index) * static_cast<uint64_t>(m_IncrementSize);
		return copyHandle;
	}
	// �q�[�v�̃A�h���X�擾
	ID3D12DescriptorHeap** GetHeapAddress() noexcept { return m_pHeap.GetAddressOf(); }
private:
	ComPtr<ID3D12DescriptorHeap> m_pHeap;
	uint32_t m_IncrementSize;
	uint32_t m_DescriptorCount;
	D3D12_CPU_DESCRIPTOR_HANDLE m_CpuHandle;
};

