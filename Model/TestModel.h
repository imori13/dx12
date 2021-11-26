#pragma once
#include "pch.h"
#include "Display.h"
#include <VertexBuffer.h>
#include <ConstantBuffer.h>

struct alignas(256) Transform
{
	DirectX::XMMATRIX World;	// ���[���h�s��
	DirectX::XMMATRIX View;		// �r���[�s��
	DirectX::XMMATRIX Proj;		// �ˉe�s��
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

