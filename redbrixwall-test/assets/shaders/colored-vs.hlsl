cbuffer ConstantBuffer : register(b0)
{
    matrix mat;
    matrix world;
}

struct VS_INPUT
{
    float4 pos : POSITION;
    float4 color : COLOR;
};
struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 worldPos : POSITION;
    float4 color : COLOR;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    
    output.pos = mul(mat, input.pos);
    output.worldPos = mul(world, input.pos);
    output.color = input.color;

    return output;
}
