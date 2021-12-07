#pragma once

namespace Command
{
	// 生成処理
	void Initialize();

	// 描画開始
	const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& Begin(uint32_t SwapIndex);
	// 描画終了
	void End();

	// 次のフレームに移動する
	void MoveToNextFrame();
	// GPU待機
	void WaitForGpu();

	ID3D12CommandQueue* GetCmdQueue() noexcept;
};