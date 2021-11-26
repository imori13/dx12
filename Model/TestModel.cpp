#include "TestModel.h"
#include "GraphicsCore.h"
#include "WinApp.h"
#include <d3dcompiler.h>

bool TestModel::OnInit()
{
	// ���_�o�b�t�@�̍쐬
	{
		struct Vertex
		{
			DirectX::XMFLOAT3 Position;	// �ʒu���W
			DirectX::XMFLOAT4 Color;	// ���_�J���[
		};

		// ���_�f�[�^
		Vertex vertices[] = {
			{ DirectX::XMFLOAT3{-1.0f,-1.0f, 0.0f}, DirectX::XMFLOAT4{ 0.0f, 0.0f, 1.0f, 1.0f}},
			{ DirectX::XMFLOAT3{ 1.0f,-1.0f, 0.0f}, DirectX::XMFLOAT4{ 0.0f, 1.0f, 0.0f, 1.0f}},
			{ DirectX::XMFLOAT3{ 0.0f, 1.0f, 0.0f}, DirectX::XMFLOAT4{ 1.0f, 0.0f, 0.0f, 1.0f}},
		};

		// �q�[�v�v���p�e�B
		D3D12_HEAP_PROPERTIES prop = {};
		// GPU�ɓ]�����邽�߂̃q�[�v�BCPU�������݂�1�x�ŁAGPU�������݂�1�x�̃f�[�^���K���Ă���
		prop.Type = D3D12_HEAP_TYPE_UPLOAD;
		prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		prop.CreationNodeMask = 1;
		prop.VisibleNodeMask = 1;

		// ���\�[�X�̐ݒ�
		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Alignment = 0;
		desc.Width = sizeof(vertices);
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_UNKNOWN;	// �o�b�t�@�̏ꍇUnknown�A�e�N�X�`���̏ꍇ�s�N�Z���t�H�[�}�b�g
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		// ���\�[�X�𐶐�
		auto hr = Graphics::g_pDevice->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(m_pVB.ReleaseAndGetAddressOf()));
		if(FAILED(hr))
		{ return false; }

		// �}�b�s���O����
		void* ptr = nullptr;
		hr = m_pVB->Map(0, nullptr, &ptr);
		if(FAILED(hr))
		{ return false; }

		// ���_�f�[�^���}�b�s���O��ɐݒ�
		memcpy(ptr, vertices, sizeof(vertices));

		// �}�b�s���O����
		m_pVB->Unmap(0, nullptr);

		// ���_�o�b�t�@�r���[�̐ݒ�
		m_VBV.BufferLocation = m_pVB->GetGPUVirtualAddress();
		m_VBV.SizeInBytes = static_cast<UINT>(sizeof(vertices));	// ���_�o�b�t�@�S�̂̃T�C�Y
		m_VBV.StrideInBytes = static_cast<UINT>(sizeof(Vertex));	// 1���_������̃T�C�Y
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
		// �q�[�v�v���p�e�B
		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_UPLOAD;	// ���_�V�F�[�_�Ŏg�p����̂�UPLOAD
		prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		prop.CreationNodeMask = 1;
		prop.VisibleNodeMask = 1;

		// ���\�[�X�̐ݒ�
		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Alignment = 0;
		desc.Width = sizeof(Transform);
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		auto incrementSize
			= Graphics::g_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		for(uint32_t i = 0u; i < FRAME_COUNT; ++i)
		{
			// ���\�[�X����
			auto hr = Graphics::g_pDevice->CreateCommittedResource(
				&prop,
				D3D12_HEAP_FLAG_NONE,
				&desc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(m_pCB[i].GetAddressOf()));
			if(FAILED(hr))
			{ return false; }

			auto address = m_pCB[i]->GetGPUVirtualAddress();
			auto handleCPU = m_pHeapCBV->GetCPUDescriptorHandleForHeapStart();
			auto handleGPU = m_pHeapCBV->GetGPUDescriptorHandleForHeapStart();

			handleCPU.ptr += incrementSize * static_cast<uint64_t>(i);
			handleGPU.ptr += incrementSize * static_cast<uint64_t>(i);

			// �萔�o�b�t�@�r���[�̐ݒ�
			m_CBV[i].HandleCPU = handleCPU;
			m_CBV[i].HandleGPU = handleGPU;
			m_CBV[i].Desc.BufferLocation = address;
			m_CBV[i].Desc.SizeInBytes = sizeof(Transform);

			// �萔�o�b�t�@�r���[�𐶐�
			Graphics::g_pDevice->CreateConstantBufferView(&m_CBV[i].Desc, handleCPU);

			// �}�b�s���O
			// MAP���\�b�h�́A�|�C���^���擾����?
			hr = m_pCB[i]->Map(0, nullptr, reinterpret_cast<void**>(&m_CBV[i].pBuffer));
			if(FAILED(hr))
			{ return false; }

			auto eyePos = DirectX::XMVectorSet(0.0f, 0.0f, 5.0f, 0.0f);
			auto targetPos = DirectX::XMVectorZero();
			auto upward = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
			auto fovY = DirectX::XMConvertToRadians(37.5f);
			auto aspect = static_cast<float>(Window::g_Width) / static_cast<float>(Window::g_Height);

			// �ϊ��s��
			m_CBV[i].pBuffer->World = DirectX::XMMatrixIdentity();
			m_CBV[i].pBuffer->View = DirectX::XMMatrixLookAtRH(eyePos, targetPos, upward);
			m_CBV[i].pBuffer->Proj = DirectX::XMMatrixPerspectiveFovRH(fovY, aspect, 1.0f, 1000.0f);
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

	return true;
}

void TestModel::Update()
{
	m_RotateAngle += 0.025f;
	m_CBV[Display::g_FrameIndex].pBuffer->World = DirectX::XMMatrixRotationY(m_RotateAngle);
}

void TestModel::Render(ID3D12GraphicsCommandList* cmdList)
{
	cmdList->SetGraphicsRootSignature(m_pRootSignature.Get());
	cmdList->SetDescriptorHeaps(1, m_pHeapCBV.GetAddressOf());
	cmdList->SetGraphicsRootConstantBufferView(0, m_CBV[Display::g_FrameIndex].Desc.BufferLocation);
	cmdList->SetPipelineState(m_pPSO.Get());
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList->IASetVertexBuffers(0, 1, &m_VBV);
	cmdList->RSSetViewports(1, &m_Viewport);
	cmdList->RSSetScissorRects(1, &m_Scissor);
	cmdList->DrawInstanced(3, 1, 0, 0);
}

void TestModel::OnTerm()
{
	for(auto i = 0; i < FRAME_COUNT; ++i)
	{
		if(m_pCB[i].Get() != nullptr)
		{
			m_pCB[i]->Unmap(0, nullptr);
			memset(&m_CBV[i], 0, sizeof(m_CBV[i]));
		}
		m_pCB[i].Reset();
	}

	m_pVB.Reset();
	m_pPSO.Reset();
}
