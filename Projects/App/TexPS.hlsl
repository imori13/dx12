#include "TexH.hlsli"

Default_PSOutput main(Default_VSOutput input)
{
    Default_PSOutput output = (Default_PSOutput) 0;
    
    float3 N = normalize(input.Normal);
    float3 L = normalize(LightDirection);
    float3 V = normalize(CameraPosition - input.WorldPos.xyz);
    
    float3 R = normalize(-reflect(V, N));
    
    float4 color = ColorMap.Sample(ColorSmp, input.TexCoord);
    float3 ambient = LightColor * Ambient;
    float3 diffuse = LightColor * Diffuse * saturate(dot(L, N));
    float3 specular = LightColor * Specular * pow(saturate(dot(L, R)), Shininess);
    
    output.Color = float4(color.rgb * Color.rgb * max(ambient, (diffuse + specular)), color.a * Color.a);

    return output;
}