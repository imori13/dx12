#pragma once
#include "UploadBuffer.h"

class VertexBuffer : public UploadBuffer
{
public:
	VertexBuffer()
		:m_BufferView{}
	{
	}

	// ê∂ê¨
	void Create(uint32_t bufferSize, uint32_t strideInBytes)
	{
		UploadBuffer::Create(bufferSize);

		m_BufferView = D3D12_VERTEX_BUFFER_VIEW{ m_GpuVirtualAddress, bufferSize, strideInBytes };
	}

	D3D12_VERTEX_BUFFER_VIEW GetView() { return m_BufferView; }
private:
	D3D12_VERTEX_BUFFER_VIEW m_BufferView;
};