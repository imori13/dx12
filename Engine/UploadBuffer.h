#pragma once
#include "GpuResource.h"

class UploadBuffer : public GpuResource
{
public:
	UploadBuffer()
		:m_BufferSize(0)
	{
	}
	~UploadBuffer()
	{
		Destroy();
	}

	// ����
	void Create(size_t bufferSize);
	// �}�b�v
	HRESULT Map(void** ptr);
	// �A���}�b�v
	void UnMap();

	// �o�b�t�@�T�C�Y���擾
	size_t GetBufferSize() const;

protected:
	size_t m_BufferSize;
};