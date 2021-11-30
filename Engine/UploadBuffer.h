#pragma once
#include "GpuResource.h"
#include "Display.h"

class UploadBuffer : public GpuResource
{
public:
	UploadBuffer() noexcept
		: m_BufferSize(0)
		, m_VertexView{}
		, m_IndexView{}
		, m_ConstantView{}
	{
	}

	// 生成
	void Create(size_t bufferSize);
	// マップ
	void* Map();
	// アンマップ
	void UnMap();

	// バッファサイズを取得
	size_t GetBufferSize() const noexcept { return m_BufferSize; }

	void CreateConstantView(uint32_t index, size_t offset, uint32_t bufferSize)
	{
		m_ConstantView.at(index) = D3D12_CONSTANT_BUFFER_VIEW_DESC{ m_GpuVirtualAddress + offset , bufferSize };
	}
	void CreateVertexView(size_t offset, uint32_t bufferSize, uint32_t elementSize) noexcept { m_VertexView = D3D12_VERTEX_BUFFER_VIEW{ m_GpuVirtualAddress + offset,  bufferSize, elementSize }; }
	void CreateIndexView(size_t offset, uint32_t bufferSize, DXGI_FORMAT format) noexcept { m_IndexView = D3D12_INDEX_BUFFER_VIEW{ m_GpuVirtualAddress + offset, bufferSize, format }; }

	const D3D12_VERTEX_BUFFER_VIEW& GetVertexView() noexcept { return m_VertexView; }
	const D3D12_INDEX_BUFFER_VIEW& GetIndexView() noexcept { return m_IndexView; }
	const D3D12_CONSTANT_BUFFER_VIEW_DESC& GetConstantView(uint32_t index) { return m_ConstantView.at(index); }

	D3D12_VERTEX_BUFFER_VIEW m_VertexView;
	D3D12_INDEX_BUFFER_VIEW m_IndexView;
	std::array<D3D12_CONSTANT_BUFFER_VIEW_DESC, FRAME_COUNT> m_ConstantView;
protected:
	size_t m_BufferSize;
};