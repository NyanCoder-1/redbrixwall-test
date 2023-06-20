struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

float vecLength(float2 vec)
{
    return sqrt(vec.x * vec.x + vec.y * vec.y);
}

static const float PI_DIV_2 = 1.5707963267948966f;

float4 main(PS_INPUT input) : SV_TARGET
{
    return float4(0.0f, 0.0f, 0.0f, lerp(0.0f, 0.2f, cos(smoothstep(0.0f, 1.0f, vecLength(input.uv)) * PI_DIV_2)));
}