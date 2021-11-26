#pragma once
#include "pch.h"
#include "Display.h"

struct alignas(256) Transform
{
	DirectX::XMMATRIX World;	// ���[���h�s��
	DirectX::XMMATRIX View;		// �r���[�s��
	DirectX::XMMATRIX Proj;		// �ˉe�s��
};

template<typename T>
struct ConstantBufferView
{
	D3D12_CONSTANT_BUFFER_VIEW_DESC Desc;	// �萔�o�b�t�@�̍\���ݒ�
	D3D12_CPU_DESCRIPTOR_HANDLE HandleCPU;	// CPU�f�B�X�N���v�^�n���h��
	D3D12_GPU_DESCRIPTOR_HANDLE HandleGPU;	// GPU�f�B�X�N���v�^�n���h��
	T* pBuffer;	// �o�b�t�@�擪�ւ̃|�C���^
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
	ComPtr<ID3D12DescriptorHeap> m_pHeapCBV;		// CBV�f�B�X�N���v�^�q�[�v
	ComPtr<ID3D12Resource> m_pVB;	// ���_�o�b�t�@
	ComPtr<ID3D12Resource> m_pCB[FRAME_COUNT];		// �萔�o�b�t�@
	ComPtr<ID3D12PipelineState> m_pPSO;	// �p�C�v���C���X�e�[�g
	ComPtr<ID3D12RootSignature> m_pRootSignature;	// ���[�g�V�O�j�`��


	D3D12_VERTEX_BUFFER_VIEW m_VBV;	// ���_�o�b�t�@�r���[
	D3D12_VIEWPORT m_Viewport;		// �r���[�|�[�g
	D3D12_RECT m_Scissor;			// �V�U�[��`
	ConstantBufferView<Transform> m_CBV[FRAME_COUNT];	// �萔�o�b�t�@�r���[
	float m_RotateAngle;	// ��]�p
};

