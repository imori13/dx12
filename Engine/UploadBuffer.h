#pragma once
#include "GpuResource.h"

class UploadBuffer : public GpuResource
{
public:
	UploadBuffer() noexcept
		:m_BufferSize(0)
	{
	}
	// 生成
	void Create(size_t bufferSize);
	// マップ
	HRESULT Map(void** ptr);
	// アンマップ
	void UnMap();

	// バッファサイズを取得
	size_t GetBufferSize() const noexcept;

protected:
	size_t m_BufferSize;
};