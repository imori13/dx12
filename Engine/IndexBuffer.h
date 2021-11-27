#pragma once
#include "UploadBuffer.h"

class IndexBuffer : public UploadBuffer
{
public:
	IndexBuffer()
		:m_BufferView{}
	{
	}

	// ê∂ê¨
	void Create(uint32_t bufferSize, DXGI_FORMAT format)
	{
		UploadBuffer::Create(bufferSize);

		m_BufferView = D3D12_INDEX_BUFFER_VIEW{ m_GpuVirtualAddress, bufferSize, format };
	}

	D3D12_INDEX_BUFFER_VIEW GetView() { return m_BufferView; }
private:
	D3D12_INDEX_BUFFER_VIEW m_BufferView;
};