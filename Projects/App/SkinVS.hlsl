#include "SkinH.hlsli"

[RootSignature(RSDEF)]
Default_VSOutput main(Default_VSInput input)
{
    Default_VSOutput output = (Default_VSOutput) 0;

    float4 totalPosition = 0;
    for (int i = 0; i < 4; i++)
    {
        if (input.boneno[i] == -1) 
            continue;
        if (input.boneno[i] >= 256)
        {
            totalPosition = float4(input.Position, 1.0f);
            break;
        }
        float4 localPosition = mul(bones[input.boneno[i]], float4(input.Position, 1.0f));
        totalPosition += localPosition * input.weight[i];
    }

    float4x4 World = float4x4(input.World0, input.World1, input.World2, input.World3);
    
    //float4 localPos = float4(input.Position, 1.0f);
    float4 worldPos = mul(World, totalPosition);
    float4 viewPos = mul(View, worldPos);
    float4 projPos = mul(Proj, viewPos);

    output.Position = projPos;
    output.TexCoord = input.TexCoord;
    output.WorldPos = worldPos;
    output.Normal = normalize(mul((float3x3) World, input.Normal));

    return output;
}