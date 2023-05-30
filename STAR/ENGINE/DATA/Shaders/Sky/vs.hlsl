cbuffer ConstantBuffer : register(b0)
{
    matrix WVP;
    float exposure;
}

struct VOut
{
    float3 world : POSITION;
    float2 texcoord : TEXCOORD;
    float4 pos : SV_POSITION;
};

VOut main(float3 pos : POSITION, float2 texcoord : TEXCOORD)
{
    VOut output;
    output.world = pos;
    output.pos = mul(float4(pos, 0.0f), WVP);
    output.texcoord = texcoord;
    return output;
}