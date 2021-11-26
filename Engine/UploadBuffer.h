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

	// 生成
	void Create(size_t bufferSize);
	// マップ
	HRESULT Map(void** ptr);
	// アンマップ
	void UnMap();

	// バッファサイズを取得
	size_t GetBufferSize() const;

protected:
	size_t m_BufferSize;
};