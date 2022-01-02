#include "NotTexH.hlsli"

Default_PSOutput main(Default_VSOutput input)
{
    Default_PSOutput output = (Default_PSOutput) 0;
    
    float3 N = normalize(input.Normal);
    float3 L = normalize(LightDirection);
    float3 V = normalize(CameraPosition - input.WorldPos.xyz);
    
    float3 R = normalize(-reflect(V, N));
    
    float3 ambient = LightColor * Ambient;
    float3 diffuse = LightColor * Diffuse * saturate(dot(L, N));
    float3 specular = LightColor * Specular * pow(saturate(dot(L, R)), 255);
    
    output.Color = float4(Color.rgb * max(ambient, (diffuse + specular)), Color.a);
    
    return output;
}