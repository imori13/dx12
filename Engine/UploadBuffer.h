#pragma once
#include "GpuResource.h"

class UploadBuffer : public GpuResource
{
public:
	virtual ~UploadBuffer() { Destroy(); }

	void Create(size_t bufferSize);

	void Map();
	void UnMap();

	size_t GetBufferSize() const { return m_BufferSize; }
protected:
	size_t m_BufferSize;
};

