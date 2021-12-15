#include "iMoriDefaultHeader.hlsli"

PSOutput main(VSOutput input)
{
    PSOutput output = (PSOutput) 0;
    
    float3 N = normalize(input.Normal);
    float3 L = normalize(LightPosition - input.WorldPos.xyz);
    
    float4 color = ColorMap.Sample(ColorSmp, input.TexCoord);
    //float3 diffuse = LightColor * Diffuse * saturate(dot(L, N));
    float3 diffuse = LightColor * 1 * saturate(dot(L, N));
    
    output.Color = float4(color.rgb * diffuse, color.a * Alpha);

    return output;
}