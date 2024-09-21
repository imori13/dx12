#include "SS_InputLayout.h"

#include <magic_enum.hpp>

namespace
{
	constexpr inline const char* ToString(SI_Semantic semantic)
	{
		return magic_enum::enum_name(semantic).data();
	}
}

void SS_InputLayout::AddElement(uint8_t inputSlot, SI_Semantic semanticName, uint8_t semanticIndex, SI_Format dataFormat, SI_Class dataClass)
{
	const auto is_instance = (dataClass == SI_Class::Instance);

	D3D12_INPUT_ELEMENT_DESC desc{};
	desc.InputSlot = inputSlot;
	desc.SemanticName = ToString(semanticName);
	desc.SemanticIndex = semanticIndex;
	desc.Format = static_cast<DXGI_FORMAT>(dataFormat);
	desc.InputSlotClass = (is_instance) ? (D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA) : (D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA);
	desc.InstanceDataStepRate = (is_instance) ? (1) : (0);
	desc.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;	// 各要素が連続する場合のelement調整

	m_InputElements.emplace_back(desc);
}

D3D12_INPUT_LAYOUT_DESC SS_InputLayout::GetLayout() const noexcept
{
	D3D12_INPUT_LAYOUT_DESC inputLayout;
	inputLayout.pInputElementDescs = m_InputElements.data();
	inputLayout.NumElements = gsl::narrow<uint32_t>(m_InputElements.size());
	return inputLayout;
}