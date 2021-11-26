#pragma once
#include "pch.h"
#include "Display.h"
#include <VertexBuffer.h>
#include <ConstantBuffer.h>

struct alignas(256) Transform
{
	DirectX::XMMATRIX World;	// ワールド行列
	DirectX::XMMATRIX View;		// ビュー行列
	DirectX::XMMATRIX Proj;		// 射影行列
};

template<typename T>
struct ConstantBufferView
{
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
	D3D12_CONSTANT_BUFFER_VIEW_DESC Desc;	// 定数バッファの構成設定
	T* pBuffer;	// バッファ先頭へのポインタ
};

class TestModel
{
public:
	TestModel()
		: m_pHeapCBV(nullptr)
		, m_pPSO(nullptr)
		, m_pRootSignature(nullptr)
		, m_Viewport{}
		, m_Scissor{}
	{
		//for(uint32_t i = 0u; i < FRAME_COUNT; ++i)
		//{
		//	m_pCB[i] = nullptr;
		//}
	}
	~TestModel()
	{}

	bool OnInit();
	void Update();
	void Render(ID3D12GraphicsCommandList* cmdList);
	void OnTerm();
private:
	ComPtr<ID3D12DescriptorHeap> m_pHeapCBV;		// CBVディスクリプタヒープ
	VertexBuffer m_pVertexBuffer;
	ConstantBuffer<Transform> m_pConstantBuffer[FRAME_COUNT];
	//ComPtr<ID3D12Resource> m_pCB[FRAME_COUNT];		// 定数バッファ
	ComPtr<ID3D12PipelineState> m_pPSO;	// パイプラインステート
	ComPtr<ID3D12RootSignature> m_pRootSignature;	// ルートシグニチャ


	struct Vertex
	{
		DirectX::XMFLOAT3 Position;	// 位置座標
		DirectX::XMFLOAT4 Color;	// 頂点カラー
	};

	// 頂点データ
	Vertex vertices[3] = {
		{ DirectX::XMFLOAT3{-1.0f,-1.0f, 0.0f}, DirectX::XMFLOAT4{ 0.0f, 0.0f, 1.0f, 1.0f}},
		{ DirectX::XMFLOAT3{ 1.0f,-1.0f, 0.0f}, DirectX::XMFLOAT4{ 0.0f, 1.0f, 0.0f, 1.0f}},
		{ DirectX::XMFLOAT3{ 0.0f, 1.0f, 0.0f}, DirectX::XMFLOAT4{ 1.0f, 0.0f, 0.0f, 1.0f}},
	};

	D3D12_VIEWPORT m_Viewport;		// ビューポート
	D3D12_RECT m_Scissor;			// シザー矩形
	//ConstantBufferView<Transform> m_CBV[FRAME_COUNT];	// 定数バッファビュー
	float m_RotateAngle;	// 回転角
};

