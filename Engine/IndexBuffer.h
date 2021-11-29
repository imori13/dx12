#pragma once
#include "UploadBuffer.h"

class IndexBuffer : public UploadBuffer
{
public:
	IndexBuffer() noexcept
		:m_BufferView{}
	{
	}

	// ê∂ê¨
	void CreateIndex(size_t bufferSize, DXGI_FORMAT format)
	{
		UploadBuffer::Create(bufferSize);

		m_BufferView = D3D12_INDEX_BUFFER_VIEW{ m_GpuVirtualAddress, gsl::narrow<uint32_t>(bufferSize), format };
	}

	const D3D12_INDEX_BUFFER_VIEW& GetView() noexcept { return m_BufferView; }
private:
	D3D12_INDEX_BUFFER_VIEW m_BufferView;
};