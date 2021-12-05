struct VSInput
{
	float3 Position : POSITION;
	float2 TexCoord : TEXCOORD;
};

struct VSOutput
{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

cbuffer Transform : register(b0)
{
	float4x4 World : packoffset(c0);
	float4x4 View : packoffset(c4);
	float4x4 Proj : packoffset(c8);
}

#define RSDEF \
	"RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)," \
	"CBV(b0, visibility= SHADER_VISIBILITY_VERTEX), " \
	"DescriptorTable(SRV(t0), visibility= SHADER_VISIBILITY_PIXEL), " \
	"StaticSampler(s0, " \
		"visibility = SHADER_VISIBILITY_PIXEL," \
		"filter= FILTER_MIN_MAG_MIP_POINT)" \

[RootSignature(RSDEF)]
VSOutput main(VSInput input)
{
	VSOutput output = (VSOutput)0;

	float4 localPos = float4(input.Position, 1.0f);
	float4 worldPos = mul(World, localPos);
	float4 viewPos = mul(View, worldPos);
	float4 projPos = mul(Proj, viewPos);

	output.Position = projPos;
	output.TexCoord = input.TexCoord;

	return output;
}