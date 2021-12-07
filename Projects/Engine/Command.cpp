#include "Command.h"
#include "Display.h"

namespace
{
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> s_pCmdQueue;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> s_pCmdList;
	std::vector<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>> s_pCmdAllocators;
	Microsoft::WRL::ComPtr<ID3D12Fence> s_pFence;

	HANDLE s_FenceEventHandle;
	std::array<uint64_t, FRAME_COUNT> s_NextFenceValue;
}

namespace Command
{
	void Initialize()
	{
		HRESULT hr{};

		// �R�}���h�L���[�̐���
		{
			// �R�}���h�L���[�̐ݒ�
			D3D12_COMMAND_QUEUE_DESC desc = {};
			desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
			desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			desc.NodeMask = 0;

			// �R�}���h�L���[�̐���
			hr = Graphics::g_pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(s_pCmdQueue.GetAddressOf()));
			ENSURES(hr, "CommandQueue����");
		}

		// �R�}���h�A���P�[�^�̐���
		{
			s_pCmdAllocators = std::vector<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>>(FRAME_COUNT);
			s_pCmdAllocators.reserve(FRAME_COUNT);
			for(auto& itr : s_pCmdAllocators)
			{
				hr = Graphics::g_pDevice->CreateCommandAllocator(
					D3D12_COMMAND_LIST_TYPE_DIRECT,
					IID_PPV_ARGS(itr.GetAddressOf()));	// �_�u���o�b�t�@�����O�p��2��
				ENSURES(hr, "CommandAllocator����");
			}
		}

		// �R�}���h���X�g�̐���
		{
			hr = Graphics::g_pDevice->CreateCommandList(
				0,
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				s_pCmdAllocators.at(0).Get(),
				nullptr,	// �p�C�v���C���X�e�[�g(��Őݒ肷��)
				IID_PPV_ARGS(s_pCmdList.GetAddressOf()));
			ENSURES(hr, "CommandList����");
		}

		// �t�F���X�̐���
		{
			// �t�F���X�̐���
			hr = Graphics::g_pDevice->CreateFence(
				static_cast<uint64_t>(0),
				D3D12_FENCE_FLAG_NONE,
				IID_PPV_ARGS(s_pFence.GetAddressOf()));
			ENSURES(hr, "Fence����");

			// �t�F���X�C�x���g�̐���
			s_FenceEventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
			ENSURES(s_FenceEventHandle != nullptr, "FenceEvent����");
		}

		s_pCmdList->Close();
	}

	ID3D12CommandQueue* GetCmdQueue() noexcept
	{
		return s_pCmdQueue.Get();
	}

	const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& Begin(uint32_t SwapIndex)
	{
		// �R�}���h�̋L�^���J�n
		auto hr = s_pCmdAllocators.at(SwapIndex)->Reset();
		ENSURES(hr);
		hr = s_pCmdList->Reset(s_pCmdAllocators.at(SwapIndex).Get(), nullptr);
		ENSURES(hr);

		return s_pCmdList;
	}

	void End()
	{
		const auto hr = s_pCmdList->Close();
		ENSURES(hr);

		ID3D12CommandList* ppCommandLists[] = { s_pCmdList.Get(), };
		s_pCmdQueue->ExecuteCommandLists(_countof(ppCommandLists), static_cast<ID3D12CommandList* const*>(ppCommandLists));
	}

	void MoveToNextFrame()
	{
		// Fence�ɒl�𑗂�
		const auto currentFenceValue = s_NextFenceValue.at(Display::g_FrameIndex);
		auto hr = s_pCmdQueue->Signal(s_pFence.Get(), currentFenceValue);
		ENSURES(hr);

		// Swapchain�̃o�b�t�@�X�V
		Display::NextFrame();

		// ���̏������ݐ�o�b�t�@���܂�GPU�Ŏg�p����Ă�����҂�
		if(s_pFence->GetCompletedValue() < s_NextFenceValue.at(Display::g_FrameIndex))
		{
			hr = s_pFence->SetEventOnCompletion(s_NextFenceValue.at(Display::g_FrameIndex), s_FenceEventHandle);
			WaitForSingleObjectEx(s_FenceEventHandle, INFINITE, FALSE);
			ENSURES(hr);
		}

		// Fence�l���X�V���ďI��
		s_NextFenceValue.at(Display::g_FrameIndex) = currentFenceValue + 1;
	}

	void WaitForGpu()
	{
		// Fence�ɒl�𑗂�
		auto hr = s_pCmdQueue->Signal(s_pFence.Get(), s_NextFenceValue.at(Display::g_FrameIndex));
		ENSURES(hr);

		//if(m_pFence->GetCompletedValue() < m_NextFenceValue[Display::g_FrameIndex])

		// Fence���X�V�����܂őҋ@
		hr = s_pFence->SetEventOnCompletion(s_NextFenceValue.at(Display::g_FrameIndex), s_FenceEventHandle);
		WaitForSingleObjectEx(s_FenceEventHandle, INFINITE, FALSE);
		ENSURES(hr);

		// Fence�l���X�V���ďI��
		s_NextFenceValue.at(Display::g_FrameIndex)++;
	}



	// private method ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	void ExecuteCommandList()
	{
		s_pCmdList->Close();

		ID3D12CommandList* ppCmdLists[] = { s_pCmdList.Get() };
		const auto span = gsl::make_span(ppCmdLists);

		s_pCmdQueue->ExecuteCommandLists(1, span.data());
	}
}