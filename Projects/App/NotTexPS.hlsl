#include "NotTexH.hlsli"

Default_PSOutput main(Default_VSOutput input)
{
    Default_PSOutput output = (Default_PSOutput) 0;
    
    float3 N = normalize(input.Normal);
    float3 L = normalize(LightPosition - input.WorldPos.xyz);
    float3 V = normalize(CameraPosition - input.WorldPos.xyz);
    
    float3 R = normalize(-reflect(V, N));
    
    float3 ambient = LightColor * 0.9;
    float3 diffuse = LightColor * 0.9 * saturate(dot(L, N));
    float3 specular = LightColor * 0.5 * pow(saturate(dot(L, R)), 0.5);

    output.Color = float4(Color.xyz * max(ambient, (diffuse + specular)), Color.a);
    
    return output;
}