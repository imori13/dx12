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
	// �v���C�x�[�g�ϐ�
	static const uint32_t FrameCount = 2;	// �t���[���o�b�t�@��

	ComPtr<ID3D12Device> m_pDevice;					// �f�o�C�X
	ComPtr<ID3D12CommandQueue> m_pQueue;			// �R�}���h�L���[
	ComPtr<IDXGISwapChain3> m_pSwapChain;			// �X���b�v�`�F�C��
	ComPtr<ID3D12Resource> m_pColorBuffer[FrameCount];			// �J���[�o�b�t�@
	ComPtr<ID3D12CommandAllocator> m_pCmdAllocator[FrameCount];	// �R�}���h�A���P�[�^
	ComPtr<ID3D12GraphicsCommandList> m_pCmdList;	// �R�}���h���X�g
	ComPtr<ID3D12DescriptorHeap> m_pHeapRTV;		// RTV�q�[�v
	ComPtr<ID3D12Fence> m_pFence;	// �t�F���X
	HANDLE m_FenceEvent;	// �t�F���X�C�x���g
	uint64_t m_FenceCounter[FrameCount];	// �t���[���J�E���^�[
	uint32_t m_FrameIndex;	// �t���[���ԍ�
	D3D12_CPU_DESCRIPTOR_HANDLE m_HandleRTV[FrameCount];	// RTV�n���h��

	// �v���C�x�[�g�֐�
	bool InitD3D();
	void TermD3D();
	void Present(uint32_t interval);
	void WaitGpu();
};

