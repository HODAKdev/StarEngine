#include "../ToneMappers/ACES.hlsl"

cbuffer ConstantBuffer : register(b0)
{
    matrix WVP;
    float exposure;
}

Texture2D sTexture2D;
SamplerState sSamplerState;

float4 main(float3 world : POSITION, float2 texcoord : TEXCOORD) : SV_TARGET
{    
    const float gamma = 2.2;
    float3 hdrColor = sTexture2D.Sample(sSamplerState, texcoord).xyz;
        // exposure tone mapping
    float3 mapped = float3(1.0, 1.0, 1.0) - exp(-hdrColor * exposure);
        // gamma correction
    //mapped = pow(mapped, float3(1.0 / gamma, 1.0 / gamma, 1.0 / gamma));
    //return float4(mapped, 1.0);
    return float4(To_ACES(hdrColor), 1.0);

}