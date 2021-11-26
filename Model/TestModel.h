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

	}
	~TestModel()
	{}

	bool OnInit();
	void Update();
	void Render(ID3D12GraphicsCommandList* cmdList);
	void OnTerm();
private:
	ComPtr<ID3D12DescriptorHeap> m_pHeapCBV;
	VertexBuffer m_pVertexBuffer;
	ConstantBuffer<Transform> m_pConstantBuffer[FRAME_COUNT];
	ComPtr<ID3D12PipelineState> m_pPSO;
	ComPtr<ID3D12RootSignature> m_pRootSignature;

	D3D12_VIEWPORT m_Viewport;
	D3D12_RECT m_Scissor;
	float m_RotateAngle;
};

