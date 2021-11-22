#pragma once

template<typename T>using ComPtr = Microsoft::WRL::ComPtr<T>;

class HogeApp
{
public:
	HogeApp();
	~HogeApp();

	bool InitApp();
	void TermApp();
	void Render();

private:
	// プライベート変数
	static const uint32_t FrameCount = 2;	// フレームバッファ数

	ComPtr<ID3D12Device> m_pDevice;					// デバイス
	ComPtr<ID3D12CommandQueue> m_pQueue;			// コマンドキュー
	ComPtr<IDXGISwapChain3> m_pSwapChain;			// スワップチェイン
	ComPtr<ID3D12Resource> m_pColorBuffer[FrameCount];			// カラーバッファ
	ComPtr<ID3D12CommandAllocator> m_pCmdAllocator[FrameCount];	// コマンドアロケータ
	ComPtr<ID3D12GraphicsCommandList> m_pCmdList;	// コマンドリスト
	ComPtr<ID3D12DescriptorHeap> m_pHeapRTV;		// RTVヒープ
	ComPtr<ID3D12Fence> m_pFence;	// フェンス
	HANDLE m_FenceEvent;	// フェンスイベント
	uint64_t m_FenceCounter[FrameCount];	// フレームカウンター
	uint32_t m_FrameIndex;	// フレーム番号
	D3D12_CPU_DESCRIPTOR_HANDLE m_HandleRTV[FrameCount];	// RTVハンドル

	// プライベート関数
	bool InitD3D();
	void TermD3D();
	void Present(uint32_t interval);
	void WaitGpu();
};

