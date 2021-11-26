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

template<typename T>
struct ConstantBufferView
{
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
	D3D12_CONSTANT_BUFFER_VIEW_DESC Desc;	// �萔�o�b�t�@�̍\���ݒ�
	T* pBuffer;	// �o�b�t�@�擪�ւ̃|�C���^
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
	ComPtr<ID3D12DescriptorHeap> m_pHeapCBV;		// CBV�f�B�X�N���v�^�q�[�v
	VertexBuffer m_pVertexBuffer;
	ConstantBuffer<Transform> m_pConstantBuffer[FRAME_COUNT];
	//ComPtr<ID3D12Resource> m_pCB[FRAME_COUNT];		// �萔�o�b�t�@
	ComPtr<ID3D12PipelineState> m_pPSO;	// �p�C�v���C���X�e�[�g
	ComPtr<ID3D12RootSignature> m_pRootSignature;	// ���[�g�V�O�j�`��


	struct Vertex
	{
		DirectX::XMFLOAT3 Position;	// �ʒu���W
		DirectX::XMFLOAT4 Color;	// ���_�J���[
	};

	// ���_�f�[�^
	Vertex vertices[3] = {
		{ DirectX::XMFLOAT3{-1.0f,-1.0f, 0.0f}, DirectX::XMFLOAT4{ 0.0f, 0.0f, 1.0f, 1.0f}},
		{ DirectX::XMFLOAT3{ 1.0f,-1.0f, 0.0f}, DirectX::XMFLOAT4{ 0.0f, 1.0f, 0.0f, 1.0f}},
		{ DirectX::XMFLOAT3{ 0.0f, 1.0f, 0.0f}, DirectX::XMFLOAT4{ 1.0f, 0.0f, 0.0f, 1.0f}},
	};

	D3D12_VIEWPORT m_Viewport;		// �r���[�|�[�g
	D3D12_RECT m_Scissor;			// �V�U�[��`
	//ConstantBufferView<Transform> m_CBV[FRAME_COUNT];	// �萔�o�b�t�@�r���[
	float m_RotateAngle;	// ��]�p
};

