//=====================================================
// Model Vertex Shader
//=====================================================

cbuffer ModelConstantBuffer : register(b0)
{
    float4x4 WorldViewProjection;

    float4 DiffuseColor;

    uint HasTexture;

    float3 Padding;
};


//=====================================================
// Input
//=====================================================

struct VSInput
{
    float3 Position : POSITION;

    float3 Normal : NORMAL;

    float2 TexCoord : TEXCOORD0;
};


//=====================================================
// Output
//=====================================================

struct VSOutput
{
    float4 Position : SV_POSITION;

    float3 Normal : NORMAL;

    float2 TexCoord : TEXCOORD0;
};


//=====================================================
// Main
//=====================================================

VSOutput main(
    VSInput input)
{
    VSOutput output;

    // C++‘¤‚ÅMatrix‚ðTranspose‚µ‚Ä‚¢‚é
    output.Position =
        mul(
            float4(
                input.Position,
                1.0f),
            WorldViewProjection);

    output.Normal =
        input.Normal;

    output.TexCoord =
        input.TexCoord;

    return output;
}