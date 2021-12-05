#pragma once

class InputElement
{
public:
	void SetElement(std::string_view semanticsName, DXGI_FORMAT format)
	{
		D3D12_INPUT_ELEMENT_DESC desc{};
		desc.SemanticName = semanticsName.data();
		desc.SemanticIndex = 0;	// POSITION0,1,2と頂点シェーダで使うなら設定する
		desc.Format = format;	// フォーマット
		desc.InputSlot = 0;		// 複数の頂点バッファ用?
		desc.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;	// 各要素が連続する場合のelement調整
		desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;	// 頂点ごとの入力データ
		desc.InstanceDataStepRate = 0;

		m_DescArrays.push_back(desc);
	}

	const gsl::span<const D3D12_INPUT_ELEMENT_DESC> Get() noexcept { return m_DescArrays; }
private:
	std::vector<D3D12_INPUT_ELEMENT_DESC> m_DescArrays;
};