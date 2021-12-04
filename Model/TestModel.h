#pragma once
#include "pch.h"
#include "Display.h"
#include "UploadBuffer.h"
#include "ResourceHeap.h"
#include "Texture.h"

struct Vertex
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT2 Texcoord;
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
	ResourceHeap m_CbvHeap;
	UploadBuffer m_UploadBuffer;
	Texture m_Texture;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pPSO;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_pRootSignature;

	Transform* m_pTransform;	// UploadBuffer�̃��������Q�Ƃ��Ă���̂ŁA�f�X�g���N�^�Ŕj�������

	D3D12_VIEWPORT m_Viewport;
	D3D12_RECT m_Scissor;
	float m_RotateAngle;
};

