#pragma once

namespace Command
{
	// ��������
	void Initialize();

	// �`��J�n
	const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& Begin(uint32_t SwapIndex);
	// �`��I��
	void End();

	// ���̃t���[���Ɉړ�����
	void MoveToNextFrame();
	// GPU�ҋ@
	void WaitForGpu();

	ID3D12CommandQueue* GetCmdQueue() noexcept;
};