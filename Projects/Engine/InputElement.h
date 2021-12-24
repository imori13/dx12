#pragma once

class InputElement
{
public:
	void SetVertexElement(std::string_view semanticsName, DXGI_FORMAT format)
	{
		D3D12_INPUT_ELEMENT_DESC desc{};
		desc.SemanticName = semanticsName.data();
		desc.SemanticIndex = 0;
		desc.Format = format;
		desc.InputSlot = 0;
		desc.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;	// 各要素が連続する場合のelement調整
		desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		desc.InstanceDataStepRate = 0;

		m_DescArrays.emplace_back(desc);
	}

	void SetInstanceElement(
	std::string_view semanticsName, DXGI_FORMAT format)
	{
		D3D12_INPUT_ELEMENT_DESC desc{};
		desc.SemanticName = semanticsName.data();
		desc.SemanticIndex = 0;
		desc.Format = format;
		desc.InputSlot = 1;
		desc.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
		desc.InstanceDataStepRate = 1;

		m_DescArrays.emplace_back(desc);
	}

	const gsl::span<const D3D12_INPUT_ELEMENT_DESC> Get() noexcept { return m_DescArrays; }
private:
	std::vector<D3D12_INPUT_ELEMENT_DESC> m_DescArrays;
};