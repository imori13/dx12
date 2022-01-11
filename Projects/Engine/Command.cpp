#include "Command.h"
#include "Display.h"
#include "Debug.h"
#include "GraphicsCore.h"
#include <omp.h>

namespace
{
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> s_pCmdQueue;
	Microsoft::WRL::ComPtr<ID3D12Fence> s_pFence;
	CommandList s_MainCmdList;
	std::vector<CommandList> s_SubCmdList;

	HANDLE s_FenceEventHandle;
	std::array<uint64_t, FRAME_COUNT> s_NextFenceValue;

	std::vector<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>> s_pBundleAllocator;
	std::vector<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>> s_pBundles;
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

		hr = s_MainCmdList.Create(FRAME_COUNT);
		ENSURES(hr, "MainCmdList����");

		// �X���b�h���m��
		s_SubCmdList.resize(omp_get_max_threads());
		for(auto i = 0; i < s_SubCmdList.size(); ++i)
		{
			hr = s_SubCmdList.at(i).Create(FRAME_COUNT);
			ENSURES(hr, "SubCmdList���� %d", i);
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
	}

	gsl::not_null<ID3D12GraphicsCommandList*> GetMainCmdList()
	{
		return s_MainCmdList.Get();
	}

	std::vector<ID3D12GraphicsCommandList*> GetSubCmdList(uint32_t)
	{
		std::vector<ID3D12GraphicsCommandList*> retVec;
		const auto size = s_SubCmdList.size();

		retVec.resize(size);
		for(auto i = 0; i < size; ++i)
		{
			retVec.at(i) = s_SubCmdList.at(i).Get();
		}
		return retVec;
	}

	gsl::not_null<ID3D12CommandQueue*> GetCmdQueue() noexcept
	{
		return s_pCmdQueue.Get();
	}

	gsl::not_null<ID3D12GraphicsCommandList*> BeginMain()
	{
		// �R�}���h�̋L�^���J�n
		const auto hr = s_MainCmdList.Reset(Display::g_FrameIndex);
		ENSURES(hr);

		return s_MainCmdList.Get();
	}

	std::vector<CommandList> BeginSub()
	{
		EndMain();
		//WaitForGpu();

		for(auto& cmdList : s_SubCmdList)
		{
			const auto hr = cmdList.Reset(Display::g_FrameIndex);
			ENSURES(hr);
		}

		return s_SubCmdList;
	}

	void EndMain()
	{
		const auto hr = s_MainCmdList.Close();
		ENSURES(hr);

		std::vector<ID3D12CommandList*> executeCmdList = { s_MainCmdList.Get()};
		s_pCmdQueue->ExecuteCommandLists(gsl::narrow<uint32_t>(executeCmdList.size()), executeCmdList.data());
	}

	void EndSub()
	{
		std::vector<ID3D12CommandList*> executeCmdList;
		executeCmdList.reserve(s_SubCmdList.size());

		for(auto& cmdList : s_SubCmdList)
		{
			const auto hr = cmdList.Close();
			ENSURES(hr);

			executeCmdList.emplace_back(cmdList.Get());
		}
		s_pCmdQueue->ExecuteCommandLists(gsl::narrow<uint32_t>(executeCmdList.size()), executeCmdList.data());

		//WaitForGpu();
		BeginMain();
	}

	void MoveToNextFrame()
	{
		// Fence�ɒl�𑗂�
		const auto currentFenceValue = s_NextFenceValue.at(Display::g_FrameIndex);
		auto hr = s_pCmdQueue->Signal(s_pFence.Get(), currentFenceValue);
		ENSURES(hr);

		// Swapchain�̃o�b�t�@�X�V
		Display::UpdateNextFrame();

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

		// Fence���X�V�����܂őҋ@
		hr = s_pFence->SetEventOnCompletion(s_NextFenceValue.at(Display::g_FrameIndex), s_FenceEventHandle);
		WaitForSingleObjectEx(s_FenceEventHandle, INFINITE, FALSE);
		ENSURES(hr);

		// Fence�l���X�V���ďI��
		s_NextFenceValue.at(Display::g_FrameIndex)++;
	}
	const gsl::not_null<ID3D12GraphicsCommandList*> CreateBandle()
	{
		auto& alocator = s_pBundleAllocator.emplace_back();
		Graphics::g_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_BUNDLE, IID_PPV_ARGS(&alocator));
		auto& bandle = s_pBundles.emplace_back();
		Graphics::g_pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_BUNDLE, alocator.Get(), nullptr, IID_PPV_ARGS(&bandle));

		return bandle.Get();
	}
}