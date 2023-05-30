cbuffer ConstantBuffer : register(b0)
{
	matrix sProjection;
	matrix sView;
	matrix sModel;
}

struct VOut
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
};

VOut main(float4 pos : POSITION, float3 normal : NORMAL, float2 texcoord : TEXCOORD)
{
    VOut output;
    output.pos = mul(pos, sModel);
    output.pos = mul(output.pos, sView);
    output.pos = mul(output.pos, sProjection);
    output.texcoord = texcoord;
    output.normal = normalize(mul(float4(normal, 0.0f), sModel));
    return output;
}