//=====================================================
// Outline Vertex Shader
//=====================================================

#define MAX_BONES 128

cbuffer OutlineConstantBuffer : register(b0)
{
    float4x4 WorldViewProjection;

    float4x4 WorldInverseTranspose;

    // x : Outline Width
    float4 OutlineParameters;

    float4 OutlineColor;
};

cbuffer BoneConstantBuffer : register(b1)
{
    float4x4 BoneMatrices[MAX_BONES];

    uint HasSkinning;

    float3 BonePadding;
};

struct VSInput
{
    float3 Position : POSITION;

    float3 Normal : NORMAL;

    float2 TexCoord : TEXCOORD0;

    int4 BoneIndices : BLENDINDICES;

    float4 BoneWeights : BLENDWEIGHT;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
};

VSOutput main(
    VSInput input)
{
    VSOutput output;

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

            skinnedPosition +=
                mul(
                    position,
                    BoneMatrices[
                        boneIndex]) *
                weight;

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
    // Outline Expansion
    //=================================================

    const float outlineWidth =
        OutlineParameters.x;

    // ModelãÛä‘Ç≈ñ@ê¸ï˚å¸Ç÷âüÇµèoÇ∑
    position.xyz +=
        normalize(normal) *
        outlineWidth;

    //=================================================
    // Transform
    //=================================================

    output.Position =
        mul(
            position,
            WorldViewProjection);

    return output;
}