cbuffer ConstantBuffer : register(b0)
{
    matrix mat;
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
    
    output.pos = mul(mat, input.pos); //float4(input.pos.x / 640.0f, input.pos.y / 360.0f, 0, 1); //mul(input.pos, mat) / 100.0f;
    output.uv = input.uv;

    return output;
}
