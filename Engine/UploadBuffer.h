#pragma once
#include "GpuResource.h"

class UploadBuffer : public GpuResource
{
public:
	UploadBuffer() noexcept
		: m_BufferSize(0)
		, m_StrideSize(0)
	{
	}

	// 生成
	void Create(size_t bufferSize, size_t strideSize);
	// マップ
	void* Map();
	// アンマップ
	void UnMap();

	const D3D12_VERTEX_BUFFER_VIEW GetVertexView() noexcept { return D3D12_VERTEX_BUFFER_VIEW{ m_GpuVirtualAddress,  m_BufferSize, m_StrideSize }; }
	const D3D12_INDEX_BUFFER_VIEW GetIndexView() noexcept { return D3D12_INDEX_BUFFER_VIEW{ m_GpuVirtualAddress, m_BufferSize, DXGI_FORMAT_R32_UINT }; }
	const D3D12_CONSTANT_BUFFER_VIEW_DESC GetConstantView() noexcept { return D3D12_CONSTANT_BUFFER_VIEW_DESC{ m_GpuVirtualAddress , m_BufferSize }; }
protected:
	uint32_t m_BufferSize;
	uint32_t m_StrideSize;
};