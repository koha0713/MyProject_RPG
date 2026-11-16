//=====================================================
// Skinned Shadow Vertex Shader
//=====================================================

#define MAX_BONES 128

cbuffer ShadowCastConstantBuffer :
	register(b0)
{
    float4x4 LightWorldViewProjection;
};

cbuffer BoneConstantBuffer :
	register(b1)
{
    float4x4 BoneMatrices[MAX_BONES];

    uint HasSkinning;

    float3 BonePadding;
};

struct VSInput
{
    float3 Position :
		POSITION;

    int4 BoneIndices :
		BLENDINDICES;

    float4 BoneWeights :
		BLENDWEIGHT;
};

float4 main(
	VSInput input) : SV_POSITION
{
    float4 position =
		float4(
			input.Position,
			1.0f);

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
        }

        position =
			skinnedPosition;
    }

    return mul(
		position,
		LightWorldViewProjection);
}