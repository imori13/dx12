#include "DefaultH.hlsli"

#define RSDEF \
	"RootFlags( ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT )," \
	"CBV( b0 , visibility = SHADER_VISIBILITY_VERTEX ), " \
	"CBV( b1 , visibility = SHADER_VISIBILITY_PIXEL ), " \
	"CBV( b2 , visibility = SHADER_VISIBILITY_PIXEL ), " \
	"StaticSampler( s0, " \
		"visibility = SHADER_VISIBILITY_PIXEL, " \
		"filter = FILTER_MIN_MAG_MIP_LINEAR )," \
	"DescriptorTable( SRV( t0 ), visibility = SHADER_VISIBILITY_PIXEL )" \

cbuffer MaterialData : register(b2)
{
    float3 Ambient : packoffset(c0);
    float3 Diffuse : packoffset(c1);
    float Alpha : packoffset(c1.w);
    float3 Specular : packoffset(c2);
    float Shininess : packoffset(c2.w);
}

SamplerState ColorSmp : register(s0);
Texture2D ColorMap : register(t0);