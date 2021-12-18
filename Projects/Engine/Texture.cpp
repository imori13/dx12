#include "Texture.h"
#include "GraphicsCore.h"
#include "Command.h"

#include <directx/d3dx12.h>

void Texture::CreateWIC(const std::wstring_view path)
{
	const gsl::not_null<ID3D12Device*> device = Graphics::g_pDevice.Get();

	DirectX::ResourceUploadBatch batch(device);
	batch.Begin();

	// Resource生成
	const auto hr = DirectX::CreateWICTextureFromFileEx(
		device,
		batch,
		path.data(),
		0,
		D3D12_RESOURCE_FLAG_NONE,
		DirectX::WIC_LOADER_FLAGS::WIC_LOADER_MIP_AUTOGEN,
		m_pResource.GetAddressOf());
	ENSURES(hr, L"テクスチャロード [ %s ]", path.data());

	// コマンド実行
	auto future = batch.End(Command::GetCmdQueue());
	future.wait();

	// texture設定取得
	const auto textureDesc = m_pResource->GetDesc();

	m_View.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	m_View.Format = textureDesc.Format;
	m_View.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	m_View.Texture2D.MostDetailedMip = 0;
	m_View.Texture2D.MipLevels = textureDesc.MipLevels;
	m_View.Texture2D.PlaneSlice = 0;
	m_View.Texture2D.ResourceMinLODClamp = 0.0f;
}

void Texture::CreateTGA(const std::wstring_view path)
{
	// 持ってきた
https://github.com/boxerprogrammer/directx12_samples/blob/master/Chapter9_Refactored2/Dx12Wrapper.cpp

	DirectX::TexMetadata meta;
	DirectX::ScratchImage image;
	auto hr = DirectX::LoadFromTGAFile(path.data(), &meta, image);
	ENSURES(hr, L"TGAファイルの取得");

	auto img = image.GetImage(0, 0, 0); //生データ抽出

	//WriteToSubresourceで転送する用のヒープ設定
	const auto texHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
	const auto resDesc = CD3DX12_RESOURCE_DESC::Tex2D(meta.format, meta.width, meta.height, meta.arraySize, meta.mipLevels);

	hr = Graphics::g_pDevice->CreateCommittedResource(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE, //特に指定なし
		&resDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(m_pResource.GetAddressOf()));
	ENSURES(hr, L"TGAファイルの読み込み");

	hr = m_pResource->WriteToSubresource(0,
	nullptr,//全領域へコピー
	img->pixels,//元データアドレス
	img->rowPitch,//1ラインサイズ
	img->slicePitch//全サイズ
	);
	ENSURES(hr, L"TGAファイルWriteToSubresource");

	// texture設定取得
	const auto textureDesc = m_pResource->GetDesc();

	m_View.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	m_View.Format = textureDesc.Format;
	m_View.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	m_View.Texture2D.MostDetailedMip = 0;
	m_View.Texture2D.MipLevels = textureDesc.MipLevels;
	m_View.Texture2D.PlaneSlice = 0;
	m_View.Texture2D.ResourceMinLODClamp = 0.0f;
}
