#pragma once

enum class SI_Semantic : uint8_t;
enum class SI_Format : uint8_t;
enum class SI_Class : uint8_t;

// シェーダーの入力データ
class SS_InputLayout
{
public:
	void AddElement(uint8_t inputSlot, SI_Semantic semanticName, uint8_t semanticIndex, SI_Format dataFormat, SI_Class dataClass);

	D3D12_INPUT_LAYOUT_DESC GetLayout() const noexcept;

private:
	std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputElements;
};

enum class SI_Semantic : uint8_t
{
	POSITION,
	NORMAL,
	TANGENT,
	TEXCOORD,
	BLENDINDICES,
	BLENDWEIGHT,
};

enum class SI_Format : uint8_t
{
	Unknown = DXGI_FORMAT_UNKNOWN,
	UInt_R_8 = DXGI_FORMAT_R8_UINT,
	UInt_RG_8 = DXGI_FORMAT_R8G8_UINT,
	UInt_R_16 = DXGI_FORMAT_R16_UINT,
	UInt_RG_16 = DXGI_FORMAT_R16G16_UINT,
	Float_R_32 = DXGI_FORMAT_R32_FLOAT,
	Float_RG_32 = DXGI_FORMAT_R32G32_FLOAT,
	Float_RGB_32 = DXGI_FORMAT_R32G32B32_FLOAT,
	Float_RGBA_32 = DXGI_FORMAT_R32G32B32A32_FLOAT,
};

enum class SI_Class : uint8_t
{
	Vertex = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
	Instance = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,
};