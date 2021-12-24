
// HLSL���s���W���[�ɕύX
#pragma pack_matrix(row_major)

struct VSInput
{
    float3 Position : POSITION; // �ʒu�x�N�g��
    float3 Normal   : NORMAL;   // �@���x�N�g��
    float2 TexCoord : TEXCOORD; // �e�N�X�`�����W
    float3 Tangent : TANGENT;   // �ڐ��x�N�g��
    
    float4 World0 : TEXCOORD1;
    float4 World1 : TEXCOORD2;
    float4 World2 : TEXCOORD3;
    float4 World3 : TEXCOORD4;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
    float3 Normal   : NORMAL;
    float4 WorldPos : WORLD_POS;
};

struct PSOutput
{
    float4 Color : SV_TARGET0;
};

#define RSDEF \
	"RootFlags( ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT )," \
	"CBV( b0 , visibility = SHADER_VISIBILITY_VERTEX ), " \
	"CBV( b1 , visibility = SHADER_VISIBILITY_PIXEL ), " \
	"CBV( b2 , visibility = SHADER_VISIBILITY_PIXEL ), " \
	"StaticSampler( s0, " \
		"visibility = SHADER_VISIBILITY_PIXEL, " \
		"filter = FILTER_MIN_MAG_MIP_LINEAR )," \
	"DescriptorTable( SRV( t0 ), visibility = SHADER_VISIBILITY_PIXEL )" \

// FILTER_MIN_MAG_MIP_POINT
// FILTER_MIN_MAG_MIP_LINEAR

//struct InstanceData
//{
//    float4x4 World : packoffset(c0);
//};

//cbuffer InstanceArray : register(b0)
//{
//    InstanceData Instances[512];
//};

// Transform (b0)
cbuffer CameraBuffer : register(b0)
{
    float4x4 View : packoffset(c0);
    float4x4 Proj : packoffset(c4);
}

// Light (b1)
cbuffer LightBuffer : register(b1)
{
    float3 LightPosition : packoffset(c0);
    float3 LightColor : packoffset(c1);
    float3 CameraPosition : packoffset(c2);
}

// Material (b2)
cbuffer MaterialBuffer : register(b2)
{
    float3 Ambient : packoffset(c0);  // �����˗�
    float3 Diffuse : packoffset(c1);  // �g�U���˗�
    float3 Specular : packoffset(c2); // ���ʔ��˗� 
    float Alpha : packoffset(c0.w);
    float Ni : packoffset(c1.w);
    float Shininess : packoffset(c2.w);
}

SamplerState ColorSmp : register(s0);
Texture2D ColorMap    : register(t0);