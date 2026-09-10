//=====================================================
// Model Vertex Shader
//=====================================================

#define MAX_BONES 128

//=====================================================
// Model ConstantBuffer
//=====================================================

cbuffer ModelConstantBuffer : register(b0)
{
    float4x4 WorldViewProjection;

    float4 DiffuseColor;

    uint HasTexture;

    float3 Padding;
};

//=====================================================
// Bone ConstantBuffer
//=====================================================

cbuffer BoneConstantBuffer : register(b1)
{
    float4x4 BoneMatrices[MAX_BONES];

    uint HasSkinning;

    float3 BonePadding;
};

//=====================================================
// Input
//=====================================================

struct VSInput
{
    float3 Position : POSITION;

    float3 Normal : NORMAL;

    float2 TexCoord : TEXCOORD0;

    int4 BoneIndices : BLENDINDICES;

    float4 BoneWeights : BLENDWEIGHT;
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

    //====================
    // 初期値
    //====================

    float4 position =
        float4(
            input.Position,
            1.0f);

    float3 normal =
        input.Normal;

    //=================================================
    // Skinning
    //=================================================
    const float totalWeight =
    input.BoneWeights.x +
    input.BoneWeights.y +
    input.BoneWeights.z +
    input.BoneWeights.w;

    if (HasSkinning != 0 &&
    totalWeight > 0.0001f)
    {
        float4 skinnedPosition =
            float4(
                0.0f,
                0.0f,
                0.0f,
                0.0f);

        float3 skinnedNormal =
            float3(
                0.0f,
                0.0f,
                0.0f);

        //====================
        // 最大4Bone
        //====================

        [unroll]
        for (int i = 0;
            i < 4;
            ++i)
        {
            const float weight =
                input.BoneWeights[i];

            if (weight <= 0.0f)
            {
                continue;
            }

            const int boneIndex =
                input.BoneIndices[i];

            if (boneIndex < 0 ||
                boneIndex >= MAX_BONES)
            {
                continue;
            }

            //====================
            // Position
            //====================

            skinnedPosition +=
                mul(
                    position,
                    BoneMatrices[
                        boneIndex]) *
                weight;

            //====================
            // Normal
            //====================

            // Normalには平行移動を適用しないので
            // float3x3へ変換して使用する。
            skinnedNormal +=
                mul(
                    normal,
                    (float3x3)
                    BoneMatrices[
                        boneIndex]) *
                weight;
        }

        position =
            skinnedPosition;

        normal =
            normalize(
                skinnedNormal);
    }

    //=================================================
    // World / View / Projection
    //=================================================

    output.Position =
        mul(
            position,
            WorldViewProjection);

    output.Normal =
        normal;

    output.TexCoord =
        input.TexCoord;

    return output;
}