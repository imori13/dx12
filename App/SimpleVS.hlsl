
// �� ����
// �uPOSITION�v�Ȃǂ̓Z�}���e�B�b�N��
struct VSInput
{
	float3 Position : POSITION;	// �ʒu���W
	float4 Color : COLOR;		// ���_�J���[
};

// �� �o��
struct VSOutput
{
	float4 Position : SV_POSITION;	// �ʒu���W
	float4 Color : COLOR;	// ���_�J���[
};

// �� Transform�萔�o�b�t�@
cbuffer Transform : register(b0)	// b0=�萔�o�b�t�@�p���W�X�^��0�Ԗ�
{
	// packoffset=c0�͐擪�Ac1��float4��(16byte)��Ac2��float8����̊J�n�ʒu
	float4x4 World : packoffset(c0);	// ���[���h�s��
	float4x4 View : packoffset(c4);		// �r���[�s��	// �ʒu:64byte
	float4x4 Proj : packoffset(c8);		// �ˉe�s��		// �ʒu:128byte
}

// �� ���_�V�F�[�_ �G���g���|�C���g
VSOutput main(VSInput input)
{
	VSOutput output = (VSOutput)0;

	float4 localPos = float4(input.Position, 1.0f);
	float4 worldPos = mul(World, localPos);
	float4 viewPos = mul(View, worldPos);
	float4 projPos = mul(Proj, viewPos);

	output.Position = projPos;
	output.Color = input.Color;

	return output;
}