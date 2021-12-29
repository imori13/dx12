// HLSLを行メジャーに変更
#pragma pack_matrix(row_major)

struct Default_VSInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
    float3 Tangent : TANGENT;
    
    float4 World0 : TEXCOORD1;
    float4 World1 : TEXCOORD2;
    float4 World2 : TEXCOORD3;
    float4 World3 : TEXCOORD4;
};

struct Default_VSOutput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
    float3 Normal : NORMAL;
    float4 WorldPos : WORLD_POS;
};

struct Default_PSOutput
{
    float4 Color : SV_TARGET0;
};

cbuffer CameraData : register(b0)
{
    float4x4 View : packoffset(c0);
    float4x4 Proj : packoffset(c4);
}

cbuffer SceneData : register(b1)
{
    float3 LightDirection : packoffset(c0);
    float3 LightColor : packoffset(c1);
    float3 CameraPosition : packoffset(c2);
    float3 CameraDirection : packoffset(c3);
}

cbuffer MaterialData : register(b2)
{
    float3 Ambient : packoffset(c0);
    float3 Diffuse : packoffset(c1);
    float Alpha : packoffset(c1.w);
    float3 Specular : packoffset(c2);
    float Shininess : packoffset(c2.w);
}