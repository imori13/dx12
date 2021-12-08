#pragma once
#include "ResourceHeap.h"
#include "UploadBuffer.h"
#include "Texture.h"
#include "PipelineStateObject.h"

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
		: m_pRootSignature(nullptr)
		, m_pTransform(nullptr)
		, m_RotateAngle(0)
	{

	}

	bool OnInit(const std::wstring& texturePath);
	void Update() noexcept;
	void Render(gsl::not_null<ID3D12GraphicsCommandList*> cmdList);
	void OnTerm() noexcept;

	float m_RotateAngle;
private:
	ResourceHeap m_CBV_SRVHeap;
	UploadBuffer m_VertexData;
	UploadBuffer m_IndexData;
	UploadBuffer m_ConstantData;
	Texture m_Texture;
	PipelineStateObject pipelineStateObject;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_pRootSignature;

	Transform* m_pTransform;	// UploadBuffer�̃��������Q�Ƃ��Ă���̂ŁA�f�X�g���N�^�Ŕj�������
};

