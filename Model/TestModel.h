#pragma once
#include "pch.h"
#include "Display.h"

struct alignas(256) Transform
{
	DirectX::XMMATRIX World;	// ワールド行列
	DirectX::XMMATRIX View;		// ビュー行列
	DirectX::XMMATRIX Proj;		// 射影行列
};

template<typename T>
struct ConstantBufferView
{
	D3D12_CONSTANT_BUFFER_VIEW_DESC Desc;	// 定数バッファの構成設定
	D3D12_CPU_DESCRIPTOR_HANDLE HandleCPU;	// CPUディスクリプタハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE HandleGPU;	// GPUディスクリプタハンドル
	T* pBuffer;	// バッファ先頭へのポインタ
};

class TestModel
{
public:
	TestModel()
		: m_pHeapCBV(nullptr)
		, m_pVB(nullptr)
		, m_pPSO(nullptr)
		, m_pRootSignature(nullptr)
	{
		for(uint32_t i = 0u; i < FRAME_COUNT; ++i)
		{
			m_pCB[i] = nullptr;
		}
	}
	~TestModel()
	{}

	bool OnInit();
	void Update();
	void Render(ID3D12GraphicsCommandList* cmdList);
	void OnTerm();
private:
	ComPtr<ID3D12DescriptorHeap> m_pHeapCBV;		// CBVディスクリプタヒープ
	ComPtr<ID3D12Resource> m_pVB;	// 頂点バッファ
	ComPtr<ID3D12Resource> m_pCB[FRAME_COUNT];		// 定数バッファ
	ComPtr<ID3D12PipelineState> m_pPSO;	// パイプラインステート
	ComPtr<ID3D12RootSignature> m_pRootSignature;	// ルートシグニチャ


	D3D12_VERTEX_BUFFER_VIEW m_VBV;	// 頂点バッファビュー
	D3D12_VIEWPORT m_Viewport;		// ビューポート
	D3D12_RECT m_Scissor;			// シザー矩形
	ConstantBufferView<Transform> m_CBV[FRAME_COUNT];	// 定数バッファビュー
	float m_RotateAngle;	// 回転角
};

