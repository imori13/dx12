#pragma once

namespace Command
{
	// ��������
	void Initialize();

	// �`��J�n
	const gsl::not_null<ID3D12GraphicsCommandList*> Begin();
	// �`��I��
	void End();

	// ���̃t���[���Ɉړ�����
	void MoveToNextFrame();
	// GPU�ҋ@
	void WaitForGpu();

	ID3D12CommandQueue* GetCmdQueue() noexcept;
};