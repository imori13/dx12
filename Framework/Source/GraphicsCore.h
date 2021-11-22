#pragma once
//
//#include "PipelineState.h"
//#include "DescriptorHeap.h"
//#include "RootSignature.h"
//#include "GraphicsCommon.h"

#include "pch.h"

namespace Graphics
{
	using namespace Microsoft::WRL;

	void Initialize(void);
	void Shutdown(void);

	extern ComPtr<ID3D12Device> m_pDevice;
}