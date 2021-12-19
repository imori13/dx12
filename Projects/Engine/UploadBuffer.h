#pragma once
#include "GpuResource.h"
#include "ResourceHeap.h"

class UploadBuffer : public GpuResource
{
public:
	UploadBuffer() noexcept
		: m_BufferSize(0)
		, m_StrideSize(0)
		, m_VertexView{}
		, m_IndexView{}
		, m_ConstantView{}
	{
	}

	// 生成
	void Create(size_t bufferSize, size_t strideSize);
	// マップ
	void* Map();
	// アンマップ
	void UnMap();

	void CreateConstantView(Handle_CPU_GPU handle);

	const D3D12_VERTEX_BUFFER_VIEW& GetVertexView() const noexcept { return m_VertexView; }
	const D3D12_INDEX_BUFFER_VIEW& GetIndexView() const noexcept { return m_IndexView; }
	const D3D12_GPU_VIRTUAL_ADDRESS& GetConstantLocation() const noexcept { return m_GpuAddress; }
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPU() const noexcept { return m_Handle.GPU; }
private:
	D3D12_VERTEX_BUFFER_VIEW m_VertexView;
	D3D12_INDEX_BUFFER_VIEW m_IndexView;
	D3D12_CONSTANT_BUFFER_VIEW_DESC m_ConstantView;

	Handle_CPU_GPU m_Handle;

	uint32_t m_BufferSize;
	uint32_t m_StrideSize;
};