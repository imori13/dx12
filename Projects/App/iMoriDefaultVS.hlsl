#include "iMoriDefaultHeader.hlsli"

[RootSignature(RSDEF)]
VSOutput main(VSInput input)
{
    VSOutput output = (VSOutput) 0;

    float4 localPos = float4(input.Position, 1.0f);
    
    localPos += float4(input.InstancePos, 1.0f);
    
    float4 worldPos = mul(World, localPos);
    float4 viewPos = mul(View, worldPos);
    float4 projPos = mul(Proj, viewPos);

    output.Position = projPos;
    output.TexCoord = input.TexCoord;
    output.WorldPos = worldPos;
    output.Normal = normalize(mul((float3x3) World, input.Normal));

    return output;
}