//=====================================================
// Model Pixel Shader
//=====================================================

cbuffer ModelConstantBuffer : register(b0)
{
    float4x4 WorldViewProjection;

    float4 DiffuseColor;
};


//=====================================================
// Input
//=====================================================

struct PSInput
{
    float4 Position : SV_POSITION;

    float3 Normal : NORMAL;

    float2 TexCoord : TEXCOORD0;
};


//=====================================================
// Main
//=====================================================

float4 main(
    PSInput input) : SV_TARGET
{
    // 現段階ではTexture / Lightingを使わない。
    // Assimpから取得したDiffuseColorのみを表示する。

    return DiffuseColor;
}