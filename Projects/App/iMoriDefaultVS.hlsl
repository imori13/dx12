#include "iMoriDefaultHeader.hlsli"

[RootSignature(RSDEF)]
VSOutput main(VSInput input)
{
    VSOutput output = (VSOutput) 0;

    float4 localPos = float4(input.Position, 1.0f);
    
    float4x4 World = float4x4(input.World0, input.World1, input.World2, input.World3);
    
    float4 worldPos = mul(World, localPos);
    float4 viewPos = mul(View, worldPos);
    float4 projPos = mul(Proj, viewPos);

    output.Position = projPos;
    output.TexCoord = input.TexCoord;
    output.WorldPos = worldPos;
    output.Normal = normalize(mul((float3x3)World, input.Normal));

    return output;
}