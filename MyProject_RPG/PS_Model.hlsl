//=====================================================
// Model Pixel Shader
//=====================================================

//=====================================================
// ConstantBuffer
//=====================================================

cbuffer ModelConstantBuffer : register(b0)
{
    float4x4 WorldViewProjection;

    float4 DiffuseColor;

    uint HasTexture;

    float3 Padding;
};

//=====================================================
// Texture
//=====================================================

// Diffuse Texture
Texture2D DiffuseTexture :
    register(t0);

// Texture Sampler
SamplerState TextureSampler :
    register(s0);

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
    // TextureÇéùÇΩÇ»Ç¢MaterialÇÕ
    // AssimpÇ©ÇÁéÊìæÇµÇΩDiffuseColorÇÃÇ›égóp
    if (HasTexture == 0)
    {
        return DiffuseColor;
    }

    //====================
    // Diffuse Texture
    //====================

    const float4 textureColor =
        DiffuseTexture.Sample(
            TextureSampler,
            input.TexCoord);

    // TextureÇ∆MaterialColorÇçáê¨
    return textureColor *
        DiffuseColor;
}