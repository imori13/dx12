#include "NotTexH.hlsli"

[RootSignature(RSDEF)]
Default_VSOutput main(Default_VSInput input)
{
    Default_VSOutput output = (Default_VSOutput) 0;

    float4 localPos = float4(input.Position, 1.0f);
    
    float4x4 World = float4x4(input.World0, input.World1, input.World2, input.World3);
    
    float4 worldPos = mul(World, localPos);
    float4 viewPos = mul(View, worldPos);
    float4 projPos = mul(Proj, viewPos);

    output.Position = projPos;
    output.WorldPos = worldPos;
    output.Normal = normalize(mul((float3x3) World, input.Normal));

    return output;
}