#include "Command.h"
#include "Display.h"
#include "Debug.h"

namespace
{
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> s_pCmdQueue;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> s_pCmdList;
	std::vector<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>> s_pCmdAllocators;
	Microsoft::WRL::ComPtr<ID3D12Fence> s_pFence;

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

		// コマンドキューの生成
		{
			// コマンドキューの設定
			D3D12_COMMAND_QUEUE_DESC desc = {};
			desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
			desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			desc.NodeMask = 0;

			// コマンドキューの生成
			hr = Graphics::g_pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(s_pCmdQueue.GetAddressOf()));
			ENSURES(hr, "CommandQueue生成");
		}

		// コマンドアロケータの生成
		{
			s_pCmdAllocators = std::vector<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>>(FRAME_COUNT);
			s_pCmdAllocators.reserve(FRAME_COUNT);
			for(auto& itr : s_pCmdAllocators)
			{
				hr = Graphics::g_pDevice->CreateCommandAllocator(
					D3D12_COMMAND_LIST_TYPE_DIRECT,
					IID_PPV_ARGS(itr.GetAddressOf()));	// ダブルバッファリング用で2個
				ENSURES(hr, "CommandAllocator生成");
			}
		}

		// コマンドリストの生成
		{
			hr = Graphics::g_pDevice->CreateCommandList(
				0,
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				s_pCmdAllocators.at(0).Get(),
				nullptr,	// パイプラインステート(後で設定する)
				IID_PPV_ARGS(s_pCmdList.GetAddressOf()));
			ENSURES(hr, "CommandList生成");
		}

		// フェンスの生成
		{
			// フェンスの生成
			hr = Graphics::g_pDevice->CreateFence(
				static_cast<uint64_t>(0),
				D3D12_FENCE_FLAG_NONE,
				IID_PPV_ARGS(s_pFence.GetAddressOf()));
			ENSURES(hr, "Fence生成");

			// フェンスイベントの生成
			s_FenceEventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
			ENSURES(s_FenceEventHandle != nullptr, "FenceEvent生成");
		}

		// バンドルアロケータ
		//Graphics::g_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_BUNDLE, IID_PPV_ARGS(&s_pBundleAllocator));

		s_pCmdList->Close();
	}

	gsl::not_null<ID3D12GraphicsCommandList*> GetCmdList() noexcept
	{
		return s_pCmdList.Get();
	}

	gsl::not_null<ID3D12CommandQueue*> GetCmdQueue() noexcept
	{
		return s_pCmdQueue.Get();
	}

	const gsl::not_null<ID3D12GraphicsCommandList*> Begin()
	{
		// コマンドの記録を開始
		auto hr = s_pCmdAllocators.at(Display::g_FrameIndex)->Reset();
		ENSURES(hr);
		hr = s_pCmdList->Reset(s_pCmdAllocators.at(Display::g_FrameIndex).Get(), nullptr);
		ENSURES(hr);

		return s_pCmdList.Get();
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
		// Fenceに値を送る
		const auto currentFenceValue = s_NextFenceValue.at(Display::g_FrameIndex);
		auto hr = s_pCmdQueue->Signal(s_pFence.Get(), currentFenceValue);
		ENSURES(hr);

		// Swapchainのバッファ更新
		Display::UpdateNextFrame();

		// 次の書き込み先バッファがまだGPUで使用されていたら待つ
		if(s_pFence->GetCompletedValue() < s_NextFenceValue.at(Display::g_FrameIndex))
		{
			hr = s_pFence->SetEventOnCompletion(s_NextFenceValue.at(Display::g_FrameIndex), s_FenceEventHandle);
			WaitForSingleObjectEx(s_FenceEventHandle, INFINITE, FALSE);
			ENSURES(hr);
		}

		// Fence値を更新して終了
		s_NextFenceValue.at(Display::g_FrameIndex) = currentFenceValue + 1;
	}

	void WaitForGpu()
	{
		// Fenceに値を送る
		auto hr = s_pCmdQueue->Signal(s_pFence.Get(), s_NextFenceValue.at(Display::g_FrameIndex));
		ENSURES(hr);

		// Fenceが更新されるまで待機
		hr = s_pFence->SetEventOnCompletion(s_NextFenceValue.at(Display::g_FrameIndex), s_FenceEventHandle);
		WaitForSingleObjectEx(s_FenceEventHandle, INFINITE, FALSE);
		ENSURES(hr);

		// Fence値を更新して終了
		s_NextFenceValue.at(Display::g_FrameIndex)++;
	}
	const gsl::not_null<ID3D12GraphicsCommandList*> CreateBandle()
	{
		//const auto maxThreadNum = omp_get_num_threads();

		auto& alocator = s_pBundleAllocator.emplace_back();
		Graphics::g_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_BUNDLE, IID_PPV_ARGS(&alocator));
		auto& bandle = s_pBundles.emplace_back();
		Graphics::g_pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_BUNDLE, alocator.Get(), nullptr, IID_PPV_ARGS(&bandle));

		return bandle.Get();
	}
}