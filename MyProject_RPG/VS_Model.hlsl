//=====================================================
// Model Vertex Shader
//=====================================================

#define MAX_BONES 128

//=====================================================
// Model ConstantBuffer
//=====================================================

cbuffer ModelConstantBuffer : register(b0)
{
    float4x4 World;
    // World * View * Projection
    float4x4 WorldViewProjection;

    // 法線変換用World逆転置行列
    float4x4 WorldInverseTranspose;

    // Material Diffuse
    float4 DiffuseColor;

    // xyz : 光が進む方向
    // w   : Intensity
    float4 LightDirectionIntensity;

    // Directional Light Color
    float4 LightColor;

    // Ambient Light
    float4 AmbientLightColor;
    
    // x : HighlightThreshold
    // y : ShadowThreshold
    // z : MidToneIntensity
    // w : ShadowIntensity
    float4 ToonParameters;

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

    // World空間の法線
    float3 WorldNormal : NORMAL;

    float2 TexCoord : TEXCOORD0;
    
    float3 WorldPosition : TEXCOORD1;
};

//=====================================================
// Main
//=====================================================

VSOutput main(
    VSInput input)
{
    VSOutput output;

    //=================================================
    // Model Space
    //=================================================

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

        // 最大4BoneのInfluence
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

            // Position Skinning
            skinnedPosition +=
                mul(
                    position,
                    BoneMatrices[
                        boneIndex]) *
                weight;

            // Normal Skinning
            //
            // Bone MatrixのTranslationは
            // Normalへ適用しない。
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
    // Position
    //=================================================

    output.Position =
        mul(
            position,
            WorldViewProjection);
    
    //=================================================
    // World Position
    //=================================================
    output.WorldPosition =
	mul(
		position,
		World).xyz;

    //=================================================
    // Normal
    //=================================================
    // Skinning後のModel Space Normalを
    // World Spaceへ変換する。

    output.WorldNormal =
        normalize(
            mul(
                normal,
                (float3x3)
                WorldInverseTranspose));

    //=================================================
    // UV
    //=================================================

    output.TexCoord =
        input.TexCoord;

    return output;
}