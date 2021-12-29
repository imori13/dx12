#include "DefaultH.hlsli"

#define RSDEF \
	"RootFlags( ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT )," \
	"CBV( b0 , visibility = SHADER_VISIBILITY_VERTEX ), " \
	"CBV( b1 , visibility = SHADER_VISIBILITY_PIXEL ), " \
	"CBV( b2 , visibility = SHADER_VISIBILITY_PIXEL ), " \

cbuffer ModelData : register(b2)
{
    float4 Color : packoffset(c0);
}