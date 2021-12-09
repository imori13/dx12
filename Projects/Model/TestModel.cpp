#include "TestModel.h"
#include "GraphicsCore.h"
#include "FileSearch.h"
#include "InputElement.h"
#include "Display.h"
#include "TextureManager.h"


bool TestModel::OnInit()
{
	m_RotateAngle = static_cast<float>(rand());
	HRESULT hr{};

	// ���_�f�[�^����
	{
		const Vertex data[] = {
				Vertex{DirectX::XMFLOAT3(-1.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f)},
				Vertex{DirectX::XMFLOAT3(1.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f) },
				Vertex{DirectX::XMFLOAT3(1.0f,-1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f)},
				Vertex{DirectX::XMFLOAT3(-1.0f,-1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f)},
		};
		const auto span = gsl::make_span(data);

		m_VertexData.Create(sizeof(data), sizeof(Vertex));
		void* ptr = m_VertexData.Map();
		memcpy(ptr, span.data(), span.size_bytes());
		m_VertexData.UnMap();
	}

	// �C���f�b�N�X�f�[�^����
	{
		const uint32_t data[] = { 0, 1, 2, 0, 2, 3 };
		const auto span = gsl::make_span(data);

		m_IndexData.Create(sizeof(data), sizeof(uint32_t));
		void* ptr = m_IndexData.Map();
		memcpy(ptr, span.data(), span.size_bytes());
		m_IndexData.UnMap();
	}

	// �萔�o�b�t�@����
	{
		m_ConstantData.Create(sizeof(Transform), sizeof(Transform));
		m_pTransform = static_cast<Transform*>(m_ConstantData.Map());
	}

	m_CBV_SRVHeap.Create(2, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

	const auto constView = m_ConstantData.GetConstantView();
	Graphics::g_pDevice->CreateConstantBufferView(&constView, m_CBV_SRVHeap.GetCPUHandle(0));


	// �� �p�C�v���C���X�e�[�g�̐���
	{
		InputElement inputElement;
		inputElement.SetElement("POSITION", DXGI_FORMAT_R32G32B32_FLOAT);
		inputElement.SetElement("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT);

		std::wstring vsPath;
		std::wstring psPath;

		if(!SearchFilePath(L"SimpleTexVS.cso", vsPath))
			ENSURES(false, L"VertexShader�p�X����");

		if(!SearchFilePath(L"SimpleTexPS.cso", psPath))
			ENSURES(false, L"PixelShader�p�X����");

		Microsoft::WRL::ComPtr<ID3DBlob> pVSBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> pPSBlob;

		// ���_�V�F�[�_�ǂݍ���
		hr = D3DReadFileToBlob(vsPath.c_str(), pVSBlob.GetAddressOf());
		ENSURES(hr, L"VertexShader�ǂݍ��� [ %s ]",vsPath.c_str());

		// �s�N�Z���V�F�[�_�ǂݍ���
		hr = D3DReadFileToBlob(psPath.c_str(), pPSBlob.GetAddressOf());
		ENSURES(hr, L"PixelShader�ǂݍ��� [ %s ]", vsPath.c_str());

		// ���[�g�V�O�l�`���ǂݍ���
		Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureBlob;
		hr = D3DGetBlobPart(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), D3D_BLOB_ROOT_SIGNATURE, 0, &rootSignatureBlob);
		ENSURES(hr, L"RootSignature�ݒ�̎擾");

		// ���[�g�V�O�l�`���ݒ�
		hr = Graphics::g_pDevice->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(), rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_pRootSignature));
		ENSURES(hr, L"RootSignature�̐���");

		// �p�C�v���C���X�e�[�g�ݒ�
		pipelineStateObject.SetInputLayout(inputElement.Get());
		pipelineStateObject.SetRootSignature(m_pRootSignature.Get());
		pipelineStateObject.SetVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize());
		pipelineStateObject.SetPixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize());
		pipelineStateObject.SetRasterizerDesc();
		pipelineStateObject.SetBlendDesc();
		pipelineStateObject.SetDepthStencil(true);

		// �p�C�v���C���X�e�[�g����
		pipelineStateObject.Create();
	}

	auto eyePos = DirectX::XMVectorSet(0.0f, 0.0f, 5.0f, 0.0f);
	auto targetPos = DirectX::XMVectorZero();
	auto upward = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	constexpr auto fovY = DirectX::XMConvertToRadians(37.5f);
	const auto aspect = static_cast<float>(Display::g_AppWidth) / static_cast<float>(Display::g_AppHeight);

	m_pTransform->World = DirectX::XMMatrixIdentity();
	m_pTransform->View = DirectX::XMMatrixLookAtRH(eyePos, targetPos, upward);
	m_pTransform->Proj = DirectX::XMMatrixPerspectiveFovRH(fovY, aspect, 1.0f, 1000.0f);

	return true;
}

void TestModel::SetTexture(const std::wstring_view textureName)
{
	m_Texture = TextureManager::GetTexture(textureName);
	m_Texture.SetHeap(m_CBV_SRVHeap, 1);
}

void TestModel::Update() noexcept
{
	auto eyePos = DirectX::XMVectorSet(0.0f, 0.0f, 5.0f, 0.0f);
	auto targetPos = DirectX::XMVectorZero();
	auto upward = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	constexpr auto fovY = DirectX::XMConvertToRadians(37.5f);

	m_pTransform->World = DirectX::XMMatrixIdentity();
	m_pTransform->World = DirectX::XMMatrixRotationY(m_RotateAngle);
	m_pTransform->View = DirectX::XMMatrixLookAtRH(eyePos, targetPos, upward);
	m_pTransform->Proj = DirectX::XMMatrixPerspectiveFovRH(fovY, Display::g_Aspect, 1.0f, 1000.0f);
}

void TestModel::Render(gsl::not_null<ID3D12GraphicsCommandList*> cmdList)
{
	cmdList->SetGraphicsRootSignature(m_pRootSignature.Get());
	cmdList->SetPipelineState(pipelineStateObject.Get());
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	const auto vertexView = m_VertexData.GetVertexView();
	cmdList->IASetVertexBuffers(0, 1, &vertexView);
	const auto indexView = m_IndexData.GetIndexView();
	cmdList->IASetIndexBuffer(&indexView);

	cmdList->SetDescriptorHeaps(1, m_CBV_SRVHeap.GetHeapAddress());
	cmdList->SetGraphicsRootConstantBufferView(0, m_ConstantData.GetConstantView().BufferLocation);
	cmdList->SetGraphicsRootDescriptorTable(1, m_Texture.GetGpuHandle());

	cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void TestModel::OnTerm() noexcept
{

}
