#pragma once
#include "pch.h"
#include "Display.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "DescriptorHeap.h"

struct Vertex
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT4 Color;
};

struct alignas(256) Transform
{
	DirectX::XMMATRIX World;	// ���[���h�s��
	DirectX::XMMATRIX View;		// �r���[�s��
	DirectX::XMMATRIX Proj;		// �ˉe�s��
};

class TestModel
{
public:
	TestModel() noexcept
		: m_pPSO(nullptr)
		, m_pRootSignature(nullptr)
		, m_Viewport{}
		, m_Scissor{}
	{

	}

	bool OnInit();
	void Update();
	void Render(gsl::not_null<ID3D12GraphicsCommandList*> cmdList);
	void OnTerm() noexcept;
private:
	DescriptorHeap m_CbvHeap;
	UploadBuffer m_UploadBuffer;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pPSO;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_pRootSignature;

	Transform* m_pTransform;

	D3D12_VIEWPORT m_Viewport;
	D3D12_RECT m_Scissor;
	float m_RotateAngle;
};

