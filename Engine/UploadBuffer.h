#pragma once
#include "GpuResource.h"

class UploadBuffer : public GpuResource
{
public:
	virtual ~UploadBuffer() { Destroy(); }

	virtual void Create(size_t bufferSize);

	HRESULT Map(void** ptr);
	void UnMap();

	size_t GetBufferSize() const { return m_BufferSize; }
protected:
	size_t m_BufferSize;
};

