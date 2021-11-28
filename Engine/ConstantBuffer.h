#pragma once
#include "UploadBuffer.h"

template<typename T>
class ConstantBuffer : public UploadBuffer
{
public:
	ConstantBuffer() noexcept
		: m_BufferView{}
	{
		
	}

	// publicïœêî
	T* m_pBuffer;

	// ê∂ê¨
	void CreateConstant(size_t bufferSize)
	{
		UploadBuffer::Create(bufferSize);
		m_BufferView = D3D12_CONSTANT_BUFFER_VIEW_DESC{ m_GpuVirtualAddress, gsl::narrow<uint32_t>(bufferSize) };
	}

	// ÉrÉÖÅ[ÇìnÇ∑
	D3D12_CONSTANT_BUFFER_VIEW_DESC GetView() noexcept { return m_BufferView; }
	
private:
	D3D12_CONSTANT_BUFFER_VIEW_DESC m_BufferView;
};