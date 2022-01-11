#pragma once
#include "CommandList.h"

namespace Command
{
	// 生成処理
	void Initialize();

	// 描画開始
	gsl::not_null<ID3D12GraphicsCommandList*> BeginMain();
	std::vector<CommandList> BeginSub();
	// 描画終了
	void EndMain();
	void EndSub();

	// 次のフレームに移動する
	void MoveToNextFrame();
	// GPU待機
	void WaitForGpu();

	const gsl::not_null<ID3D12GraphicsCommandList*> CreateBandle();

	gsl::not_null<ID3D12GraphicsCommandList*> GetMainCmdList();
	std::vector<ID3D12GraphicsCommandList*> GetSubCmdList(uint32_t threadNum);
	gsl::not_null<ID3D12CommandQueue*> GetCmdQueue() noexcept;
};