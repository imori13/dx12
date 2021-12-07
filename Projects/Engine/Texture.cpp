#include "Texture.h"
#include "FileSearch.h"
#include "GraphicsCore.h"

void Texture::Create(const std::wstring& path)
{
	std::wstring texturePath;
	const bool find = SearchFilePath(path.c_str(), texturePath);
	ENSURES(find, _T("�t�@�C������:%s"), path.c_str());

	const gsl::not_null<ID3D12Device*> device = Graphics::g_pDevice.Get();

	DirectX::ResourceUploadBatch batch(device);
	batch.Begin();

	// Resource����
	const auto hr = DirectX::CreateDDSTextureFromFile(
		device,
		batch,
		texturePath.c_str(),
		m_pResource.GetAddressOf());
	ENSURES(hr, _T("�e�N�X�`������:%s"), texturePath.c_str());

	// �R�}���h���s
	auto future = batch.End(Graphics::g_Command.GetCmdQueue());
	future.wait();

	// texture�ݒ�擾
	const auto textureDesc = m_pResource->GetDesc();

	m_View.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	m_View.Format = textureDesc.Format;
	m_View.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	m_View.Texture2D.MostDetailedMip = 0;
	m_View.Texture2D.MipLevels = textureDesc.MipLevels;
	m_View.Texture2D.PlaneSlice = 0;
	m_View.Texture2D.ResourceMinLODClamp = 0.0f;
}

void Texture::CreateWIC(const std::wstring& path)
{
	std::wstring texturePath;
	const bool find = SearchFilePath(path.c_str(), texturePath);
	ENSURES(find, _T("�t�@�C������:%s"), path.c_str());

	const gsl::not_null<ID3D12Device*> device = Graphics::g_pDevice.Get();

	DirectX::ResourceUploadBatch batch(device);
	batch.Begin();

	// Resource����
	const auto hr = DirectX::CreateWICTextureFromFileEx(
		device,
		batch,
		texturePath.c_str(),
		0,
		D3D12_RESOURCE_FLAG_NONE,
		DirectX::WIC_LOADER_FLAGS::WIC_LOADER_FORCE_SRGB | DirectX::WIC_LOADER_FLAGS::WIC_LOADER_MIP_AUTOGEN,
		m_pResource.GetAddressOf());
	ENSURES(hr, _T("�e�N�X�`������:%s"), texturePath.c_str());

	// �R�}���h���s
	auto future = batch.End(Graphics::g_Command.GetCmdQueue());
	future.wait();

	// texture�ݒ�擾
	const auto textureDesc = m_pResource->GetDesc();

	m_View.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	m_View.Format = textureDesc.Format;
	m_View.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	m_View.Texture2D.MostDetailedMip = 0;
	m_View.Texture2D.MipLevels = textureDesc.MipLevels;
	m_View.Texture2D.PlaneSlice = 0;
	m_View.Texture2D.ResourceMinLODClamp = 0.0f;
}
