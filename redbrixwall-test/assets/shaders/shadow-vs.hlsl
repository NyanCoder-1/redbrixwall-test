cbuffer ConstantBuffer : register(b0)
{
    matrix mat;
    matrix world;
}

struct VS_INPUT
{
    float4 pos : POSITION;
    float2 uv : TEXCOORD;
};
struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    
    output.pos = mul(mat, input.pos);
    output.uv = input.uv;

    return output;
}
