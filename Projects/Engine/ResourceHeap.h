#pragma once

class ResourceHeap
{
public:
	ResourceHeap() noexcept
		: m_pHeap(nullptr)
		, m_HeapHandle_CPU{}
		, m_HeapHandle_GPU{}
		, m_DescriptorCount(0)
		, m_IncrementSize(0)
	{}

	// 生成
	void Create(uint32_t bufferCount, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flag);

	// 指定のハンドル取得
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(uint32_t index);
	// ヒープのアドレス取得
	ID3D12DescriptorHeap** GetHeapAddress() noexcept { return m_pHeap.GetAddressOf(); }
private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_pHeap;
	uint32_t m_IncrementSize;
	uint32_t m_DescriptorCount;
	D3D12_CPU_DESCRIPTOR_HANDLE m_HeapHandle_CPU;
	D3D12_GPU_DESCRIPTOR_HANDLE m_HeapHandle_GPU;
};

