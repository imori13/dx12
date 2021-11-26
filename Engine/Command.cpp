#include "Command.h"
#include "GraphicsCore.h"

void Command::Create(uint32_t swapCount)
{
	HRESULT hr;

	// �R�}���h�L���[�̐���
	{
		// �R�}���h�L���[�̐ݒ�
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask = 0;

		// �R�}���h�L���[�̐���
		hr = Graphics::g_pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(m_pCmdQueue.GetAddressOf()));
		if(FAILED(hr))
		{ return; }
	}

	// �R�}���h�A���P�[�^�̐���
	{
		m_pCmdAllocators = std::vector<ComPtr<ID3D12CommandAllocator>>(swapCount);
		m_pCmdAllocators.reserve(swapCount);

		for(auto i = 0u; i < swapCount; ++i)
		{

			hr = Graphics::g_pDevice->CreateCommandAllocator(
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				IID_PPV_ARGS(m_pCmdAllocators[i].GetAddressOf()));	// �_�u���o�b�t�@�����O�p��2��
			if(FAILED(hr))
			{ return; }
		}
	}

	// �R�}���h���X�g�̐���
	{
		hr = Graphics::g_pDevice->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			m_pCmdAllocators[0].Get(),
			nullptr,	// �p�C�v���C���X�e�[�g(��Őݒ肷��)
			IID_PPV_ARGS(m_pCmdList.GetAddressOf()));
		if(FAILED(hr))
		{ return; }
	}

	// �t�F���X�̐���
	{
		// �t�F���X�̐���
		hr = Graphics::g_pDevice->CreateFence(
			static_cast<UINT64>(0),
			D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(m_pFence.GetAddressOf()));
		if(FAILED(hr))
		{ return; }

		// �t�F���X�C�x���g�̐���
		m_FenceEventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if(m_FenceEventHandle == nullptr)
		{ return; }
	}

	m_pCmdList->Close();
}

const ComPtr<ID3D12GraphicsCommandList>& Command::Begin(uint32_t SwapIndex)
{
	ClearCommand(SwapIndex);

	return m_pCmdList;
}

void Command::Finish()
{
	auto fenceValue = ExecuteCommandList();
	WaitForFence(fenceValue);
}

// private method ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void Command::ClearCommand(uint32_t SwapIndex)
{
	// �R�}���h�̋L�^���J�n
	m_pCmdAllocators[SwapIndex]->Reset();
	m_pCmdList->Reset(m_pCmdAllocators[SwapIndex].Get(), nullptr);
}

// TODO : �}���`�o�b�t�@�ɑΉ�����(�������ݒ���̃o�b�t�@�͑҂����A�������ݗ\��̃o�b�t�@�����҂Ă΂���)
uint64_t Command::SignalQueue()
{
	m_pCmdQueue->Signal(m_pFence.Get(), m_NextFenceValue);
	return m_NextFenceValue++;
}

uint64_t Command::ExecuteCommandList()
{
	m_pCmdList->Close();

	ID3D12CommandList* ppCmdLists[] = { m_pCmdList.Get() };
	m_pCmdQueue->ExecuteCommandLists(1, ppCmdLists);
	
	return SignalQueue();
}

void Command::WaitForGpu()
{
	auto fenceValue = SignalQueue();

	WaitForFence(fenceValue);
}

void Command::WaitForFence(uint64_t fenceValue)
{
	if(m_pFence->GetCompletedValue() >= fenceValue)
	{ return; }

	m_pFence->SetEventOnCompletion(fenceValue, m_FenceEventHandle);
	WaitForSingleObjectEx(m_FenceEventHandle, INFINITE, FALSE);
}

