#pragma once

class InputElement
{
public:
	void SetElement(std::string_view semanticsName, DXGI_FORMAT format)
	{
		D3D12_INPUT_ELEMENT_DESC desc{};
		desc.SemanticName = semanticsName.data();
		desc.SemanticIndex = 0;	// POSITION0,1,2�ƒ��_�V�F�[�_�Ŏg���Ȃ�ݒ肷��
		desc.Format = format;	// �t�H�[�}�b�g
		desc.InputSlot = 0;		// �����̒��_�o�b�t�@�p?
		desc.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;	// �e�v�f���A������ꍇ��element����
		desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;	// ���_���Ƃ̓��̓f�[�^
		desc.InstanceDataStepRate = 0;

		m_DescArrays.push_back(desc);
	}

	const gsl::span<const D3D12_INPUT_ELEMENT_DESC> Get() noexcept { return m_DescArrays; }
private:
	std::vector<D3D12_INPUT_ELEMENT_DESC> m_DescArrays;
};