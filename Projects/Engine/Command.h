#pragma once
#include "CommandList.h"

namespace Command
{
	// ��������
	void Initialize();

	// �`��J�n
	gsl::not_null<ID3D12GraphicsCommandList*> BeginMain();
	std::vector<CommandList> BeginSub();
	// �`��I��
	void EndMain();
	void EndSub();

	// ���̃t���[���Ɉړ�����
	void MoveToNextFrame();
	// GPU�ҋ@
	void WaitForGpu();

	const gsl::not_null<ID3D12GraphicsCommandList*> CreateBandle();

	gsl::not_null<ID3D12GraphicsCommandList*> GetMainCmdList();
	std::vector<ID3D12GraphicsCommandList*> GetSubCmdList(uint32_t threadNum);
	gsl::not_null<ID3D12CommandQueue*> GetCmdQueue() noexcept;
};