#include "TestModel.h"
#include "GraphicsCore.h"
#include "WinApp.h"
#include <d3dcompiler.h>

bool TestModel::OnInit()
{
	// ���_�o�b�t�@�̐���
	{
		struct Vertex
		{
			DirectX::XMFLOAT3 Position;
			DirectX::XMFLOAT4 Color;
		};
		Vertex vertices[4] = {
				{ DirectX::XMFLOAT3{-1.0f, 1.0f, 0.0f}, DirectX::XMFLOAT4{ 1.0f, 0.0f, 0.0f, 1.0f}},
				{ DirectX::XMFLOAT3{ 1.0f, 1.0f, 0.0f}, DirectX::XMFLOAT4{ 0.0f, 1.0f, 0.0f, 1.0f}},
				{ DirectX::XMFLOAT3{ 1.0f,-1.0f, 0.0f}, DirectX::XMFLOAT4{ 0.0f, 0.0f, 1.0f, 1.0f}},
				{ DirectX::XMFLOAT3{-1.0f,-1.0f, 0.0f}, DirectX::XMFLOAT4{ 1.0f, 0.0f, 1.0f, 1.0f}},
		};

		m_pVertexBuffer.Create(static_cast<uint32_t>(sizeof(vertices)), static_cast<uint32_t>(sizeof(Vertex)));

		void* ptr = nullptr;
		m_pVertexBuffer.Map(&ptr);

		memcpy(ptr, vertices, sizeof(vertices));

		m_pVertexBuffer.UnMap();
	}

	// �C���f�b�N�X�o�b�t�@�̐���
	{
		uint32_t indices[] = { 0,1,2,0,2,3 };

		m_pIndexBuffer.Create(static_cast<uint32_t>(sizeof(indices)), DXGI_FORMAT_R32_UINT);

		void* ptr = nullptr;
		m_pIndexBuffer.Map(&ptr);

		memcpy(ptr, indices, sizeof(indices));

		m_pIndexBuffer.UnMap();
	}

	// �� �萔�o�b�t�@�p�f�B�X�N���v�^�q�[�v�̐���
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors = 1 * FRAME_COUNT;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		desc.NodeMask = 0;

		auto hr = Graphics::g_pDevice->CreateDescriptorHeap(
			&desc,
			IID_PPV_ARGS(m_pHeapCBV.GetAddressOf()));
		if(FAILED(hr))
		{ return false; }
	}

	// �� �萔�o�b�t�@�̐���
	{
		// �萔�o�b�t�@�r���[�𐶐�
		auto handleCPU = m_pHeapCBV->GetCPUDescriptorHandleForHeapStart();
		auto incrementSize = Graphics::g_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		for(uint32_t i = 0u; i < FRAME_COUNT; ++i)
		{
			m_pConstantBuffer[i].Create(sizeof(Transform));

			m_pConstantBuffer[i].Map(reinterpret_cast<void**>(&m_pConstantBuffer[i].m_pBuffer));

			handleCPU.ptr += incrementSize * static_cast<uint64_t>(i);

			auto bufferView = m_pConstantBuffer[i].GetView();
			Graphics::g_pDevice->CreateConstantBufferView(&bufferView, handleCPU);
		}

		// �� ���[�g�V�O�j�`���̐���
		{
			// ���[�g�V�O�l�`�������_�V�F�[�_�\�݂̂��g�p����̂ŁA�œK�����₷���悤�Ƀt���O��ݒ�
			auto flag = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
			flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
			flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
			flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

			// ���[�g�p�����[�^�̐ݒ�
			D3D12_ROOT_PARAMETER rootparam = {};
			rootparam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			rootparam.Descriptor.ShaderRegister = 0;
			rootparam.Descriptor.RegisterSpace = 0;
			rootparam.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

			// ���[�g�V�O�l�`���̐ݒ�
			D3D12_ROOT_SIGNATURE_DESC rootdesc = {};
			rootdesc.NumParameters = 1;
			rootdesc.NumStaticSamplers = 0;
			rootdesc.pParameters = &rootparam;
			rootdesc.pStaticSamplers = nullptr;
			rootdesc.Flags = flag;

			ComPtr<ID3DBlob> pBlob;
			ComPtr<ID3DBlob> pErrorBlob;

			// �V���A���C�Y (����)
			auto hr = D3D12SerializeRootSignature(
				&rootdesc,
				D3D_ROOT_SIGNATURE_VERSION_1_0,
				pBlob.GetAddressOf(),
				pErrorBlob.GetAddressOf());
			if(FAILED(hr))
			{ return false; }

			// ���[�g�V�O�j�`��
			hr = Graphics::g_pDevice->CreateRootSignature(
				0,
				pBlob->GetBufferPointer(),
				pBlob->GetBufferSize(),
				IID_PPV_ARGS(m_pRootSignature.GetAddressOf()));
			if(FAILED(hr))
			{ return false; }
		}

		// �� �p�C�v���C���X�e�[�g�̐���
		{
			// ���̓��C�A�E�g�̐ݒ� (���_�V�F�[�_�ƒ萔�o�b�t�@�̕R�Â�)
			D3D12_INPUT_ELEMENT_DESC elements[2];
			elements[0].SemanticName = "POSITION";
			elements[0].SemanticIndex = 0;	// POSITION0,1,2�ƒ��_�V�F�[�_�Ŏg���Ȃ�ݒ肷��
			elements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;	// �t�H�[�}�b�g
			elements[0].InputSlot = 0;		// �����̒��_�o�b�t�@�p?
			elements[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;	// �e�v�f���A������ꍇ��element����
			elements[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;	// ���_���Ƃ̓��̓f�[�^
			elements[0].InstanceDataStepRate = 0;

			elements[1].SemanticName = "COLOR";
			elements[1].SemanticIndex = 0;
			elements[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			elements[1].InputSlot = 0;
			elements[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
			elements[1].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			elements[1].InstanceDataStepRate = 0;

			// ���X�^���C�U�[�X�e�[�g�̐ݒ�
			D3D12_RASTERIZER_DESC descRS;
			descRS.FillMode = D3D12_FILL_MODE_SOLID;		// ���C���[�t���[��or�h��Ԃ�
			descRS.CullMode = D3D12_CULL_MODE_NONE;			// ���ʃJ�����Oon/off
			descRS.FrontCounterClockwise = FALSE;			// ���v��蒸�_��O�ʂƂ���
			descRS.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;	// �s�N�Z���[�x�����B����Z�\�[�g�p
			descRS.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
			descRS.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
			descRS.DepthClipEnable = FALSE;			// �����Ɋ�Â��N���b�s���O
			descRS.MultisampleEnable = FALSE;		// �}���`�T���v�����Oon/off
			descRS.AntialiasedLineEnable = FALSE;	// �A���`�G�C���A�V���Oon/off
			descRS.ForcedSampleCount = 0;
			descRS.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;	// ���X�^���C�U�̓h�͈͍L����

			// �����_�[�^�[�Q�b�g�̃u�����h�ݒ�(�������Ȃǂ��\)
			D3D12_RENDER_TARGET_BLEND_DESC descRTBS = {
				FALSE, FALSE,
				D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
				D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
				D3D12_LOGIC_OP_NOOP,
				D3D12_COLOR_WRITE_ENABLE_ALL
			};

			// �u�����h�X�e�[�g�̐ݒ�
			D3D12_BLEND_DESC descBS;
			descBS.AlphaToCoverageEnable = FALSE;
			descBS.IndependentBlendEnable = FALSE;
			for(UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
			{ descBS.RenderTarget[i] = descRTBS; }

			ComPtr<ID3DBlob> pVSBlob;
			ComPtr<ID3DBlob> pPSBlob;

			// ���_�V�F�[�_�ǂݍ���
			auto hr = D3DReadFileToBlob(L"SimpleVS.cso", pVSBlob.GetAddressOf());
			if(FAILED(hr))
			{ return false; }

			// �s�N�Z���V�F�[�_�ǂݍ���
			hr = D3DReadFileToBlob(L"SimplePS.cso", pPSBlob.GetAddressOf());
			if(FAILED(hr))
			{ return false; }

			// �p�C�v���C���X�e�[�g�̐ݒ�
			D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineState = {};
			pipelineState.InputLayout = { elements, _countof(elements) };
			pipelineState.pRootSignature = m_pRootSignature.Get();
			pipelineState.VS = { pVSBlob->GetBufferPointer(),pVSBlob->GetBufferSize() };
			pipelineState.PS = { pPSBlob->GetBufferPointer(),pPSBlob->GetBufferSize() };
			pipelineState.RasterizerState = descRS;
			pipelineState.BlendState = descBS;
			pipelineState.DepthStencilState.DepthEnable = FALSE;
			pipelineState.DepthStencilState.StencilEnable = FALSE;
			pipelineState.SampleMask = UINT_MAX;
			pipelineState.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			pipelineState.NumRenderTargets = 1;
			pipelineState.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			pipelineState.DSVFormat = DXGI_FORMAT_UNKNOWN;
			pipelineState.SampleDesc.Count = 1;
			pipelineState.SampleDesc.Quality = 0;

			// �p�C�v���C���X�e�[�g�𐶐�
			hr = Graphics::g_pDevice->CreateGraphicsPipelineState(
				&pipelineState,
				IID_PPV_ARGS(m_pPSO.GetAddressOf()));
			if(FAILED(hr))
			{ return false; }
		}

		// �� �r���[�|�[�g�ƃV�U�[��`�̐ݒ�
		{
			m_Viewport.TopLeftX = 0;
			m_Viewport.TopLeftY = 0;
			m_Viewport.Width = static_cast<float>(Window::g_Width);
			m_Viewport.Height = static_cast<float>(Window::g_Height);
			m_Viewport.MinDepth = 0.0f;
			m_Viewport.MaxDepth = 1.0f;
			m_Scissor.left = 0;
			m_Scissor.right = Window::g_Width;
			m_Scissor.top = 0;
			m_Scissor.bottom = Window::g_Height;
		}
	}

	// Transform�̏�����
	for(uint32_t i = 0u; i < FRAME_COUNT; ++i)
	{
		auto eyePos = DirectX::XMVectorSet(0.0f, 0.0f, 5.0f, 0.0f);
		auto targetPos = DirectX::XMVectorZero();
		auto upward = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		constexpr auto fovY = DirectX::XMConvertToRadians(37.5f);
		auto aspect = static_cast<float>(Window::g_Width) / static_cast<float>(Window::g_Height);

		// �ϊ��s��
		m_pConstantBuffer[i].m_pBuffer->World = DirectX::XMMatrixIdentity();
		m_pConstantBuffer[i].m_pBuffer->View = DirectX::XMMatrixLookAtRH(eyePos, targetPos, upward);
		m_pConstantBuffer[i].m_pBuffer->Proj = DirectX::XMMatrixPerspectiveFovRH(fovY, aspect, 1.0f, 1000.0f);
	}

	return true;
}

void TestModel::Update()
{
	// Transform�̍X�V
	m_RotateAngle += 0.025f;
	m_pConstantBuffer[Display::g_FrameIndex].m_pBuffer->World = DirectX::XMMatrixRotationY(m_RotateAngle);
}

void TestModel::Render(ID3D12GraphicsCommandList* cmdList)
{
	cmdList->SetGraphicsRootSignature(m_pRootSignature.Get());
	cmdList->SetDescriptorHeaps(1, m_pHeapCBV.GetAddressOf());
	cmdList->SetGraphicsRootConstantBufferView(0, m_pConstantBuffer[Display::g_FrameIndex].GetView().BufferLocation);
	cmdList->SetPipelineState(m_pPSO.Get());
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	auto vertexView = m_pVertexBuffer.GetView();
	cmdList->IASetVertexBuffers(0, 1, &vertexView);
	auto indexView = m_pIndexBuffer.GetView();
	cmdList->IASetIndexBuffer(&indexView);
	cmdList->RSSetViewports(1, &m_Viewport);
	cmdList->RSSetScissorRects(1, &m_Scissor);
	cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void TestModel::OnTerm()
{
	for(uint32_t i = 0u; i < FRAME_COUNT; ++i)
	{
		m_pConstantBuffer[i].Destroy();
	}
	m_pIndexBuffer.Destroy();
	m_pVertexBuffer.Destroy();
	m_pPSO.Reset();
}
