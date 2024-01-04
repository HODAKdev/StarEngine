#include "../ToneMappers/ACES.hlsl"
#include "../ToneMappers/sRGB.hlsl"

SamplerState sSamplerState;
Texture2D sTexture2D;

float4 main(float4 pos : SV_POSITION, float3 normal : NORMAL, float2 texcoord : TEXCOORD) : SV_TARGET
{
    float3 lightPos = float3(2.0f, 2.0f, -2.0f);
    float3 lightColor = float3(1.0f, 0.8f, 1.0f);
    float3 objectColor = float3(0.8f, 1.0f, 0.8f);

    // ambient
    float ambientStrength = 0.4;
    float3 ambient = ambientStrength * lightColor;

    // diffuse 
    float3 norm = normalize(normal);
    float3 pos_norm = normalize(pos);
    float3 lightDir = normalize(lightPos - float3(pos_norm.x, pos_norm.y, pos_norm.z));
    float diff = max(dot(norm, lightDir), 0.0);
    //float3 diffuse = diff * sTexture2D.Sample(sSamplerState, texcoord).xyz;
    float3 diffuse = diff * lightColor;
    float3 result = (ambient + diffuse) * objectColor;
    //return sTexture2D.Sample(sSamplerState, texcoord);
    float3 colx = sTexture2D.Sample(sSamplerState, texcoord);
    //return float4(To_ACES(result.xyz * colx.xyz), 1.0);
    return float4(To_sRGB(colx), 1.0);
    //return float4(lerp(sTexture2D.Sample(sSamplerState, texcoord).xyz, result, 0.5), 1.0);
}