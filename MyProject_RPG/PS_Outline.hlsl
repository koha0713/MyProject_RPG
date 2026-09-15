//=====================================================
// Outline Pixel Shader
//=====================================================

cbuffer OutlineConstantBuffer : register(b0)
{
    float4x4 WorldViewProjection;

    float4x4 WorldInverseTranspose;

    float4 OutlineParameters;

    float4 OutlineColor;
};

struct PSInput
{
    float4 Position : SV_POSITION;
};

float4 main(
    PSInput input) : SV_TARGET
{
    return OutlineColor;
}