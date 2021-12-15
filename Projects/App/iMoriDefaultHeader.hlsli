struct VSInput
{
    float3 Position : POSITION; // 位置ベクトル
    float3 Normal   : NORMAL;   // 法線ベクトル
    float2 TexCoord : TEXCOORD; // テクスチャ座標
    float3 Tangent  : TANGENT;  // 接線ベクトル
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
		"filter = FILTER_MIN_MAG_MIP_POINT )," \
	"DescriptorTable( SRV( t0 ), visibility = SHADER_VISIBILITY_PIXEL )" \

// Transform (b0)
cbuffer Transform : register(b0)
{
    float4x4 World : packoffset(c0);
    float4x4 View  : packoffset(c4);
    float4x4 Proj  : packoffset(c8);
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
    float3 Ambient : packoffset(c0);  // 環境反射率
    float3 Diffuse : packoffset(c1);  // 拡散反射率
    float3 Specular : packoffset(c2); // 鏡面反射率 
    float Alpha : packoffset(c0.w);
    float Ni : packoffset(c1.w);
    float Shininess : packoffset(c2.w);
}

SamplerState ColorSmp : register(s0);
Texture2D ColorMap    : register(t0);